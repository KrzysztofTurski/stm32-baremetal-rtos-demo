/* osKernel.h (MINIMUM) */
#ifndef OSKERNEL_H_
#define OSKERNEL_H_

#include <stdint.h>

void osKernelInit(void);
void osKernelLaunch(uint32_t quanta_ms);

//uint8_t osKernelAddThreads();


void osThreadYield(void);
void osThreadSleep(uint32_t sleep_ms);

#endif
