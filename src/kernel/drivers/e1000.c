#include <drivers/e1000.h>
#include <printk.h>
#include <task/task.h>
#include <interrupt.h>
#include <memory/memory_manager.h>
#include <drivers/io.h>
#include <lib/include/string.h>
#include <drivers/mio.h>

#define PCI_CONF_ADDR 0xCF8
#define PCI_CONF_DATA 0xCFC

// 网卡设备的范围 0x1000 ～ 0x1028
#define NET_DEVICEID_LOW 0x1000
#define NET_DEVICEID_HIGH 0x1028

#define IRQ_NIC 0x2b       // 网卡

#define PCI_BAR_NR 6

// 寄存器偏移
enum REGISTERS
{
    E1000_CTRL = 0x00,   // Device Control 设备控制
    E1000_STATUS = 0x08, // Device Status 设备状态
    E1000_EERD = 0x14,   // EEPROM Read EEPROM 读取

    E1000_ICR = 0xC0, // Interrupt Cause Read 中断原因读
    E1000_ITR = 0xC4, // Interrupt Throttling 中断节流
    E1000_ICS = 0xC8, // Interrupt Cause Set 中断原因设置
    E1000_IMS = 0xD0, // Interrupt Mask Set/Read 中断掩码设置/读
    E1000_IMC = 0xD8, // Interrupt Mask Clear 中断掩码清除

    E1000_RCTL = 0x100,   // Receive Control 接收控制
    E1000_RDBAL = 0x2800, // Receive Descriptor Base Address LOW 接收描述符低地址
    E1000_RDBAH = 0x2804, // Receive Descriptor Base Address HIGH 64bit only 接收描述符高地址
    E1000_RDLEN = 0x2808, // Receive Descriptor Length 接收描述符长度
    E1000_RDH = 0x2810,   // Receive Descriptor Head 接收描述符头
    E1000_RDT = 0x2818,   // Receive Descriptor Tail 接收描述符尾

    E1000_TCTL = 0x400,   // Transmit Control 发送控制
    E1000_TDBAL = 0x3800, // Transmit Descriptor Base Low 传输描述符低地址
    E1000_TDBAH = 0x3804, // Transmit Descriptor Base High 传输描述符高地址
    E1000_TDLEN = 0x3808, // Transmit Descriptor Length 传输描述符长度
    E1000_TDH = 0x3810,   // TDH Transmit Descriptor Head 传输描述符头
    E1000_TDT = 0x3818,   // TDT Transmit Descriptor Tail 传输描述符尾

    E1000_MAT0 = 0x5200, // Multicast Table Array 05200h-053FCh 组播表数组
    E1000_MAT1 = 0x5400, // Multicast Table Array 05200h-053FCh 组播表数组
};

#define PCI_ADDR(bus, dev, func, addr) ((u32_t)(0x80000000) | ((bus & 0xff) << 16) | ((dev & 0x1f) << 11) | ((func & 0x7) << 8) | addr)

static pci_device_t pci_net_device;

static inline u32_t pci_indw(u8_t bus, u8_t dev, u8_t func, u8_t addr)
{
    outdw(PCI_CONF_ADDR, PCI_ADDR(bus, dev, func, addr));
    return indw(PCI_CONF_DATA);
}

void pci_outdw(u8_t bus, u8_t dev, u8_t func, u8_t addr, u32_t value)
{
    outdw(PCI_CONF_ADDR, PCI_ADDR(bus, dev, func, addr));
    outdw(PCI_CONF_DATA, value);
}

#define NAME_LEN 16

// 中断类型
enum IMS
{
    // 传输描述符写回，表示有一个数据包发出
    IM_TXDW = 1 << 0, // Transmit Descriptor Written Back.

    // 传输队列为空
    IM_TXQE = 1 << 1, // Transmit Queue Empty.

    // 连接状态变化，可以认为是网线拔掉或者插上
    IM_LSC = 1 << 2, // Link Status Change

    // 接收序列错误
    IM_RXSEQ = 1 << 3, // Receive Sequence Error.

    // 到达接受描述符最小阈值，表示流量太大，接收描述符太少了，应该再多加一些，不过没有数据包丢失
    IM_RXDMT0 = 1 << 4, // Receive Descriptor Minimum Threshold hit.

    // 因为没有可用的接收缓冲区或因为PCI接收带宽不足，已经溢出，有数据包丢失
    IM_RXO = 1 << 6, // Receiver FIFO Overrun.

    // 接收定时器中断
    IM_RXT0 = 1 << 7, // Receiver Timer Interrupt.

    // 这个位在 MDI/O 访问完成时设置
    IM_MADC = 1 << 9, // MDI/O Access Complete Interrupt

    IM_RXCFG = 1 << 10,  // Receiving /C/ ordered sets.
    IM_PHYINT = 1 << 12, // Sets mask for PHY Interrupt
    IM_GPI0 = 1 << 13,   // General Purpose Interrupts.
    IM_GPI1 = 1 << 14,   // General Purpose Interrupts.

    // 传输描述符环已达到传输描述符控制寄存器中指定的阈值。
    IM_TXDLOW = 1 << 15, // Transmit Descriptor Low Threshold hit
    IM_SRPD = 1 << 16,   // Small Receive Packet Detection
};

// 接收控制
enum RCTL
{
    RCTL_EN = 1 << 1,               // Receiver Enable
    RCTL_SBP = 1 << 2,              // Store Bad Packets
    RCTL_UPE = 1 << 3,              // Unicast Promiscuous Enabled
    RCTL_MPE = 1 << 4,              // Multicast Promiscuous Enabled
    RCTL_LPE = 1 << 5,              // Long Packet Reception Enable
    RCTL_LBM_NONE = 0b00 << 6,      // No Loopback
    RCTL_LBM_PHY = 0b11 << 6,       // PHY or external SerDesc loopback
    RTCL_RDMTS_HALF = 0b00 << 8,    // Free Buffer Threshold is 1/2 of RDLEN
    RTCL_RDMTS_QUARTER = 0b01 << 8, // Free Buffer Threshold is 1/4 of RDLEN
    RTCL_RDMTS_EIGHTH = 0b10 << 8,  // Free Buffer Threshold is 1/8 of RDLEN

    RCTL_BAM = 1 << 15, // Broadcast Accept Mode
    RCTL_VFE = 1 << 18, // VLAN Filter Enable

    RCTL_CFIEN = 1 << 19, // Canonical Form Indicator Enable
    RCTL_CFI = 1 << 20,   // Canonical Form Indicator Bit Value
    RCTL_DPF = 1 << 22,   // Discard Pause Frames
    RCTL_PMCF = 1 << 23,  // Pass MAC Control Frames
    RCTL_SECRC = 1 << 26, // Strip Ethernet CRC

    RCTL_BSIZE_256 = 3 << 16,
    RCTL_BSIZE_512 = 2 << 16,
    RCTL_BSIZE_1024 = 1 << 16,
    RCTL_BSIZE_2048 = 0 << 16,
    RCTL_BSIZE_4096 = (3 << 16) | (1 << 25),
    RCTL_BSIZE_8192 = (2 << 16) | (1 << 25),
    RCTL_BSIZE_16384 = (1 << 16) | (1 << 25),
};

// 传输控制
enum TCTL
{
    TCTL_EN = 1 << 1,      // Transmit Enable
    TCTL_PSP = 1 << 3,     // Pad Short Packets
    TCTL_CT = 4,           // Collision Threshold
    TCTL_COLD = 12,        // Collision Distance
    TCTL_SWXOFF = 1 << 22, // Software XOFF Transmission
    TCTL_RTLC = 1 << 24,   // Re-transmit on Late Collision
    TCTL_NRTU = 1 << 25,   // No Re-transmit on underrun
};

// 接收状态
enum RS
{
    RS_DD = 1 << 0,    // Descriptor done
    RS_EOP = 1 << 1,   // End of packet
    RS_VP = 1 << 3,    // Packet is 802.1q (matched VET);
                       // indicates strip VLAN in 802.1q packet
    RS_UDPCS = 1 << 4, // UDP checksum calculated on packet
    RS_L4CS = 1 << 5,  // L4 (UDP or TCP) checksum calculated on packet
    RS_IPCS = 1 << 6,  // Ipv4 checksum calculated on packet
    RS_PIF = 1 << 7,   // Passed in-exact filter
};

// 传输命令
enum TCMD
{
    TCMD_EOP = 1 << 0,  // End of Packet
    TCMD_IFCS = 1 << 1, // Insert FCS
    TCMD_IC = 1 << 2,   // Insert Checksum
    TCMD_RS = 1 << 3,   // Report Status
    TCMD_RPS = 1 << 4,  // Report Packet Sent
    TCMD_VLE = 1 << 6,  // VLAN Packet Enable
    TCMD_IDE = 1 << 7,  // Interrupt Delay Enable
};

// 发送状态
enum TS
{
    TS_DD = 1 << 0, // Descriptor Done
    TS_EC = 1 << 1, // Excess Collisions
    TS_LC = 1 << 2, // Late Collision
    TS_TU = 1 << 3, // Transmit Underrun
};

#define RX_DESC_NR 32 // 接收描述符数量
#define TX_DESC_NR 32 // 传输描述符数量

// 接收描述符
typedef struct rx_desc_t
{
    u64_t addr;     // 地址
    u16_t length;   // 长度
    u16_t checksum; // 校验和
    u8_t status;    // 状态
    u8_t error;     // 错误
    u16_t special;  // 特殊
} __packed rx_desc_t;

// 传输描述符
typedef struct tx_desc_t
{
    u64_t addr;    // 缓冲区地址
    u16_t length;  // 包长度
    u8_t cso;      // Checksum Offset
    u8_t cmd;      // 命令
    u8_t status;   // 状态
    u8_t css;      // Checksum Start Field
    u16_t special; // 特殊
} __packed tx_desc_t;

typedef struct e1000_t
{
    char name[NAME_LEN]; // 名称

    pci_device_t *device; // PCI 设备
    u32_t membase;          // 映射内存基地址

    u8_t mac[6];   // MAC 地址
    bool link;   // 网络连接状态
    bool eeprom; // 只读存储器可用

    rx_desc_t *rx_desc; // 接收描述符
    u16_t rx_cur;         // 接收描述符指针

    tx_desc_t *tx_desc; // 传输描述符
    u16_t tx_cur;         // 传输描述符指针
    task_t *tx_waiter;  // 传输等待进程
} e1000_t;

static e1000_t obj;

static u32_t pci_size(u32_t base, u32_t mask)
{
    // 去掉必须设置的低位
    u32_t size = mask & base;

    // 按位取反再加1得到大小
    size = ~size + 1;

    return size;
}

// 去 pci 总线里面找网络设备
static pci_device_t* find_net_device()
{
    u32_t value = 0;
    for (int bus = 0; bus < 256; bus++)
    {
        for (int dev = 0; dev < 32; dev++)
        {
            for (u8_t func = 0; func < 8; func++)
            {
                value = pci_indw(bus, dev, func, PCI_CONF_VENDOR);
                u16_t vendorid = value & 0xffff;
                if (vendorid == 0 || vendorid == 0xFFFF)
                    continue;

                pci_net_device.bus = bus;
                pci_net_device.dev = dev;
                pci_net_device.func = func;

                pci_net_device.vendorid = vendorid;
                pci_net_device.deviceid = value >> 16;

                value = pci_indw(bus, dev, func, PCI_CONF_REVISION);
                pci_net_device.classcode = value >> 8;
                pci_net_device.revision = value & 0xff;

                if (pci_net_device.deviceid >= NET_DEVICEID_LOW && pci_net_device.deviceid <= NET_DEVICEID_HIGH)
                {
                    printk("pci net device :%d\r\n", pci_net_device.classcode);
                    return &pci_net_device;
                }
            }
        }
    }
}

u32_t pci_find_bar(pci_device_t *device, pci_bar_t *bar)
{
    for (size_t idx = 0; idx < PCI_BAR_NR; idx++)
    {
        u8_t addr = PCI_CONF_BASE_ADDR0 + (idx << 2);
        u32_t value = pci_indw(device->bus, device->dev, device->func, addr);
        pci_outdw(device->bus, device->dev, device->func, addr, -1);
        u32_t len = pci_indw(device->bus, device->dev, device->func, addr);
        pci_outdw(device->bus, device->dev, device->func, addr, value);

        if (value == 0)
            continue;

        if (len == 0 || len == -1)
            continue;

        if (value == -1)
            value = 0;

        if (!(value & 1))
        {
            bar->iobase = value & PCI_BAR_MEM_MASK;
            bar->size = pci_size(len, PCI_BAR_MEM_MASK);
            return OK;
        }
    }
    return ERR;
}

// 启用总线主控，用于发起 DMA
void pci_enable_busmastering(pci_device_t *device)
{
    u32_t data = pci_indw(device->bus, device->dev, device->func, PCI_CONF_COMMAND);
    data |= PCI_COMMAND_MASTER;
    pci_outdw(device->bus, device->dev, device->func, PCI_CONF_COMMAND, data);
}

extern void interrupt_handler_e1000();

// 系统调用处理函数
void do_interrupt_handler_e1000(exception_frame_t* exception_frame) {
    printk("---->");
}

// 检测只读存储器
static void e1000_eeprom_detect(e1000_t *e1000)
{
    moutdw(e1000->membase + E1000_EERD, 0x1);
    for (size_t i = 0; i < 1000; i++)
    {
        u32_t val = mindw(e1000->membase + E1000_EERD);
        if (val & 0x10)
            e1000->eeprom = true;
        else
            e1000->eeprom = false;
    }
}

// 读取只读存储器
static u16_t e1000_eeprom_read(e1000_t *e1000, u8_t addr)
{
    u32_t tmp;
    if (e1000->eeprom)
    {
        moutdw(e1000->membase + E1000_EERD, 1 | (u32_t)addr << 8);
        while (!((tmp = mindw(e1000->membase + E1000_EERD)) & (1 << 4)))
            ;
    }
    else
    {
        moutdw(e1000->membase + E1000_EERD, 1 | (u32_t)addr << 2);
        while (!((tmp = mindw(e1000->membase + E1000_EERD)) & (1 << 1)))
            ;
    }
    return (tmp >> 16) & 0xFFFF;
}

// 读取 MAC 地址
static void e1000_read_mac(e1000_t *e1000)
{
    e1000_eeprom_detect(e1000);
    if (e1000->eeprom)
    {
        u16_t val;
        val = e1000_eeprom_read(e1000, 0);
        e1000->mac[0] = val & 0xFF;
        e1000->mac[1] = val >> 8;

        val = e1000_eeprom_read(e1000, 1);
        e1000->mac[2] = val & 0xFF;
        e1000->mac[3] = val >> 8;

        val = e1000_eeprom_read(e1000, 2);
        e1000->mac[4] = val & 0xFF;
        e1000->mac[5] = val >> 8;
    }
    else
    {
        char *mac = (char *)e1000->membase + 0x5400;
        for (int i = 5; i >= 0; i--)
        {
            e1000->mac[i] = mac[i];
        }
    }
}

// 重置网卡
static void e1000_reset(e1000_t *e1000)
{
    e1000_read_mac(e1000);

    // 初始化组播表数组
    for (int i = E1000_MAT0; i < E1000_MAT1; i += 4)
        moutdw(e1000->membase + i, 0);

    // 禁用中断
    moutdw(e1000->membase + E1000_IMS, 0);

    // 接收初始化
    e1000->rx_desc = (rx_desc_t *)alloc_a_page(1); // TODO: free
    e1000->rx_cur = 0;
    moutdw(e1000->membase + E1000_RDBAL, (u32_t)e1000->rx_desc);
    moutdw(e1000->membase + E1000_RDLEN, sizeof(rx_desc_t) * RX_DESC_NR);

    // 接收描述符头尾指针
    moutdw(e1000->membase + E1000_RDH, 0);
    moutdw(e1000->membase + E1000_RDT, RX_DESC_NR - 1);

    // 接收描述符地址
    for (size_t i = 0; i < RX_DESC_NR; i++)
    {
        e1000->rx_desc[i].addr = (u64_t)alloc_a_page(); // TODO: free
        e1000->rx_desc[i].status = 0;
    }

    // 接收控制寄存器
    u32_t value = 0;
    value |= RCTL_EN | RCTL_SBP | RCTL_UPE;
    value |= RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF;
    value |= RCTL_BAM | RCTL_SECRC | RCTL_BSIZE_2048;
    moutdw(e1000->membase + E1000_RCTL, value);

    // 传输初始化
    e1000->tx_desc = (tx_desc_t *)alloc_a_page(); // TODO:free
    e1000->tx_cur = 0;
    moutdw(e1000->membase + E1000_TDBAL, (u32_t)e1000->tx_desc);
    moutdw(e1000->membase + E1000_TDLEN, sizeof(tx_desc_t) * TX_DESC_NR);

    // 传输描述符头尾指针
    moutdw(e1000->membase + E1000_TDH, 0);
    moutdw(e1000->membase + E1000_TDT, 0);

    // 传输描述符基地址
    for (size_t i = 0; i < TX_DESC_NR; i++)
    {
        e1000->tx_desc[i].addr = (u64_t)alloc_a_page(); // TODO: free
        e1000->tx_desc[i].status = TS_DD;
    }

    // 传输控制寄存器
    value = 0;
    value |= TCTL_EN | TCTL_PSP | TCTL_RTLC;
    value |= 0x10 << TCTL_CT;
    value |= 0x40 << TCTL_COLD;
    moutdw(e1000->membase + E1000_TCTL, value);

    // 初始化中断
    value = 0;
    value = IM_RXT0 | IM_RXO | IM_RXDMT0 | IM_RXSEQ | IM_LSC;
    value |= IM_TXQE | IM_TXDW | IM_TXDLOW;
    moutdw(e1000->membase + E1000_IMS, value);
}

void send_packet(eth_t *eth, u16_t len)
{
    e1000_t *e1000 = &obj;
    tx_desc_t *tx = &e1000->tx_desc[e1000->tx_cur];
    while (tx->status == 0)
    {
        //assert(e1000->tx_waiter == NULL);
        //e1000->tx_waiter = running_task();
        //assert(task_block(e1000->tx_waiter, NULL, TASK_BLOCKED, TIMELESS) == OK);
    }

    memcpy((void *)(u32_t)tx->addr, eth, len);

    tx->length = len;
    tx->cmd = TCMD_EOP | TCMD_RS | TCMD_RPS | TCMD_IFCS;
    tx->status = 0;
    e1000->tx_cur = (e1000->tx_cur + 1) % TX_DESC_NR;
    moutdw(e1000->membase + E1000_TDT, e1000->tx_cur);

    /*printk("ETH S [0x%04X]: %m -> %m, %d\n",
         ntohs(eth->type),
         eth->src,
         eth->dst,
         len);*/
}

// 发送测试数据包
void test_e1000_send_packet()
{
    e1000_t *e1000 = &obj;
    eth_t *eth = (eth_t *)alloc_a_page();
    memcpy(eth->src, e1000->mac, 6);
    memcpy(eth->dst, "\xff\xff\xff\xff\xff\xff", 6);
    eth->type = 0x0090; // LOOP 0x9000

    int len = 1500;
    memset(eth->payload, 'A', len);
    send_packet(eth, len + sizeof(eth_t));
    free_a_page(eth);
}

void net_init()
{
    pci_device_t* net_device = find_net_device();
    e1000_t *e1000 = &obj;

    strcpy(e1000->name, "e1000");

    e1000->device = net_device;

    pci_enable_busmastering(net_device);

    pci_bar_t membar;
    u32_t ret = pci_find_bar(net_device, &membar);
    assert(ret == OK);

    printk("e1000 membase 0x%x size %d\n", membar.iobase, membar.size);

    // 映射物理内存区域
    e1000->membase = membar.iobase;

    e1000_reset(e1000);

    // 设置中断处理函数
    register_interrupt_handler(IRQ_NIC, interrupt_handler_e1000);
    open_hardware_interrupt(IRQ_NIC);
    
    for (size_t i = 0; i < 10; i++)
    {
        test_e1000_send_packet();
    }
    
}
