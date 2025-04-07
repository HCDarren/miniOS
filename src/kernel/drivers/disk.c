#include <drivers/disk.h>
#include <drivers/io.h>
#include <printk.h>

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

// IDE 控制器状态寄存器
#define DISK_SR_NULL 0x00 // NULL

/**
 最多应该是有 4 块磁盘，总共有两个通道，每个通道两个盘（主盘和从盘）
 */
#define DISK_COUNT 4
static disk_t disk_tab[DISK_COUNT] = {0};

static disk_t* fat16_disk;

static inline void disk_set_sector(const disk_t* disk, const u32_t lba_start, const u32_t count) {

}

static inline void disk_write_data(const disk_t* disk, void* buf, const u32_t size) {

}

static inline void disk_read_data(const disk_t* disk, u16_t * buf, const u32_t size) {
    for (int i = 0; i < size / 2; i++) {
        *buf++ = inw(DISK_DATA(disk));
    }
}

// 等待空闲
static int disk_wait_idle(disk_t* disk, u8_t mask) {
    u8_t status;
    while (true)
    {
        // 错误就没有处理了，应该要处理的
        status = inb(DISK_STATUS(disk));
        if ((status & mask) == mask) // 等待的状态完成
            return 0;
    }
    return EOF;
}

// 识别硬盘
static int disk_identify(disk_t* disk) {
    outb(DISK_HDDEVSEL(disk), disk->disk_type == DISK_MASTER ? DISK_LBA_MASTER : DISK_LBA_SLAVE);
    outb(DISK_COMMAND(disk), DISK_CMD_IDENTIFY);

    int err = inb(DISK_STATUS(disk));
    if (err == 0) {
        return -1;
    }

    disk_wait_idle(disk, DISK_SR_NULL);
    // 512 个字节，扇区数量在 200 的地方
    u16_t buf[256];
    disk_read_data(disk, buf, sizeof(buf));
    disk->lba_size = *(u32_t *)(buf + 100);
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

    // 这里还有分区，还有各种文件系统代码，我这里忽略了，感兴趣大家可以自己写下
    // 我为了快速学习，这块我放弃了，时间精力有限，当时学习只有规划了一年的时间，我直接用 fat16 了，而且都写死
    fat16_disk = &disk_tab[0];
    printk("fat16_disk: name = %s, size = %dM\r\n", fat16_disk->name, fat16_disk->lba_size * 512/1024/1024);
}