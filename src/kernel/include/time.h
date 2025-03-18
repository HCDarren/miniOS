#ifndef MINIOS_TIME_H
#define MINIOS_TIME_H

#include <os.h>

// 1000 / 100 = 10ms
#define HZ 100
// 振荡器固定的
#define OSCILLATOR 1193182 
#define CLOCK_COUNTER (OSCILLATOR / HZ)
#define JIFFY (1000 / HZ)

void time_init(void);

#endif