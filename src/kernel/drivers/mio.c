#include <drivers/mio.h>

u8_t minb(u32_t addr)
{
    return *((volatile u8_t *)addr);
}

u16_t minw(u32_t addr)
{
    return *((volatile u16_t *)addr);
}

u32_t mindw(u32_t addr)
{
    return *((volatile u32_t *)addr);
}

void moutb(u32_t addr, u8_t value)
{
    *((volatile u8_t *)addr) = value;
}

void moutw(u32_t addr, u16_t value)
{
    *((volatile u16_t *)addr) = value;
}

void moutdw(u32_t addr, u32_t value)
{
    *((volatile u32_t *)addr) = value;
}
