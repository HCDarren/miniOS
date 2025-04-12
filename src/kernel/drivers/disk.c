#include <drivers/disk.h>
#include <drivers/io.h>
#include <printk.h>
#include <base/string.h>
#include <drivers/device.h>
#include <fs/fat16.h>
#include <base/assert.h>

#define DISK_DATA(disk) (disk->io_port_base + 0x0000)       // 数据寄存器
#define DISK_ERR(disk) (disk->io_port_base +0x0001)         // 错误寄存器
#define DISK_FEATURE(disk) (disk->io_port_base +0x0001)     // 功能寄存器
#define DISK_SECTOR(disk) (disk->io_port_base +0x0002)      // 扇区数量
#define DISK_LBA_LOW(disk) (disk->io_port_base +0x0003)     // LBA 低字节
#define DISK_LBA_MID(disk) (disk->io_port_base +0x0004)     // LBA 中字节
#define DISK_LBA_HIGH(disk) (disk->io_port_base +0x0005)    // LBA 高字节
#define DISK_HDDEVSEL(disk) (disk->io_port_base +0x0006)    // 磁盘选择寄存器
#define DISK_STATUS(disk) (disk->io_port_base +0x0007)      // 状态寄存器
#define DISK_COMMAND(disk) (disk->io_port_base +0x0007)     // 命令寄存器

#define DISK_LBA_MASTER 0b11100000 // 主盘 LBA
#define DISK_LBA_SLAVE 0b11110000  // 从盘 LBA

// 状态寄存器
#define DISK_STATUS_ERR          (1 << 0)    // 发生了错误
#define DISK_STATUS_DRQ          (1 << 3)    // 准备好接受数据或者输出数据
#define DISK_STATUS_DF           (1 << 5)    // 驱动错误
#define DISK_STATUS_BUSY         (1 << 7)    // 正忙

#define	DISK_DRIVE_BASE		    0xE0

#define	DISK_DISK_MASTER (0 << 4)     // 主设备
#define	DISK_DISK_SLAVE (1 << 4)      // 从设备

// IDE 控制器状态寄存器
#define DISK_SR_OK 0x08

/**
 最多应该是有 4 块磁盘，总共有两个通道，每个通道两个盘（主盘和从盘）
 */
#define DISK_COUNT 4
static disk_t disk_tab[DISK_COUNT] = {0};

static disk_t* fat16_disk;

/* 3、如何读取硬盘数据
|端口  |	作用|
|-----|------|
|0x1F0|数据端口|
|0x1F1|错误信息端口|
|0x1F2|指定读取或写入的扇区数|
|0x1F3|lba地址的低8位|
|0x1F4|lba地址的中8位|
|0x1F5|lba地址的高8位|
|0x1F6|混合位，见下面详细解释|
|0x1F7|命令位和状态位|
**0x1F6:**
- 前4位是 LBA28 的最高 4 位。
- 第5位 device 是位，0 位主盘，1 为副盘
- 第6位固定值为 1
- 第7位，LBA模式为 1，CHS模式为 0
- 第8位固定值为 1
*/
static inline void disk_select_sector_send_cmd(const disk_t* disk, const u32_t lba_start, const u32_t sector_count, const u32_t cmd) {
    // 设置 LBA48 参数（分两次写入高/低 24 位）
    // --- 低 24 位 ---
    outb(DISK_SECTOR(disk), (sector_count >> 8) & 0xFF); // 扇区数高字节（LBA48）
    outb(DISK_LBA_LOW(disk), (lba_start >> 24) & 0xFF); // LBA 24-31
    outb(DISK_LBA_MID(disk), 0); // LBA 32-39
    outb(DISK_LBA_HIGH(disk), 0); // LBA 40-47
    // --- 高 24 位 ---
    outb(DISK_SECTOR(disk), sector_count & 0xFF); // 扇区数低字节
    outb(DISK_LBA_LOW(disk), lba_start & 0xFF); // LBA 0-7
    outb(DISK_LBA_MID(disk), (lba_start >> 8) & 0xFF); // LBA 8-15
    outb(DISK_LBA_HIGH(disk), (lba_start >> 16) & 0xFF); // LBA 16-23

    // 设置驱动器为 LBA 模式 + 主盘
    outb(DISK_HDDEVSEL(disk), (disk->disk_type == DISK_MASTER ? DISK_DISK_MASTER : DISK_DISK_SLAVE) | DISK_DRIVE_BASE); // 0x40 表示 LBA 模式
    // 命令端口
    outb(DISK_COMMAND(disk), cmd);
}

static inline void disk_write_data(const disk_t* disk, void* buf, const u32_t size) {

}

static inline void disk_read_data(const disk_t* disk, u16_t * buf, const u32_t size) {
    for (int i = 0; i < size / 2; i++) {
        *buf++ = inw(DISK_DATA(disk));
    }
}

// 等待空闲
static inline int disk_wait_idle(disk_t* disk) {
    u8_t status;
    while (true)
    {
        // 错误就没有处理了，应该要处理的
        status = inb(DISK_STATUS(disk));
        if ((status & (DISK_STATUS_BUSY | DISK_STATUS_DRQ | DISK_STATUS_ERR)) != DISK_STATUS_BUSY)  // 等待的状态完成
            break;
    }
    return (status & DISK_STATUS_ERR) ? ERR : 0;
}

// 识别硬盘
static int disk_identify(disk_t* disk) {
    disk_select_sector_send_cmd(disk, 0, 0, DISK_CMD_IDENTIFY);
    int err = inb(DISK_STATUS(disk));
    if (err == 0) {
        return ERR;
    }
    err = disk_wait_idle(disk);
    if (err != 0) {
        return err;
    }
    // 512 个字节，扇区数量在 200 的地方
    u16_t buf[256];
    disk_read_data(disk, buf, sizeof(buf));
    disk->lba_size = *(u32_t *)(buf + 100);

    // 读取并设置分区表，第一个扇区 64 字节
    disk_select_sector_send_cmd(disk, 0, 1, DISK_CMD_READ);
    err = disk_wait_idle(disk);
    if (err != 0) {
        return err;
    }
    disk_read_data(disk, buf, sizeof(buf));
    disk_partition_info_t* partition_info_start = (disk_partition_info_t*)((u8_t*)buf + 446);
    for (u32_t i = 0; i < PARTITION_NUMBER; i++, partition_info_start++)
    {
        sprintf(disk->partitiont_table[i].name, "%s%d", disk->name, i);
        disk->partitiont_table[i].partition_type = partition_info_start->partition_type;
        disk->partitiont_table[i].lba_start = partition_info_start->lba_start;
        disk->partitiont_table[i].sector_count = partition_info_start->sector_count;
    }
    return 0;
}

void disk_init(void) {
    for (u32_t i = 0; i < DISK_COUNT; i++)
    {
        disk_t* disk = &disk_tab[i];
        sprintf(disk->name, "sd%d", i);
        // 0:主通道主盘；1:主通道从盘；2:从通道主盘；3:从通道从盘；
        disk->io_port_base = (i / 2 == 0) ? DISK_IOBASE_PRIMARY : DISK_IOBASE_SECONDARY;
        disk->disk_type = (i % 2 == 0) ? DISK_MASTER : DISK_SLAVE;
        disk_identify(disk);
    }
    // mac 电脑不太好弄分区，我直接用 
    // brew install dosfstools
    // mkfs.fat -F 16 slave.img
    // hdiutil attach slave.img -mountpoint /Volumes/mydisk
    // 搞了一整块的 slave.img 用的 fat16，本来打算用 ext 格式的
    fat16_init(&disk_tab[1]);
}

void disk_io_read(const disk_t* disk, u32_t lba_start, u32_t count, u16_t * buf, u32_t size) {
    disk_select_sector_send_cmd(disk, lba_start, count, DISK_CMD_READ);
    u32_t err = disk_wait_idle(disk);
    disk_read_data(disk, buf, size);
    assert(err == 0);
}