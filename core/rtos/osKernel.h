/* osKernel.h (MINIMUM) */
#ifndef OSKERNEL_H_
#define OSKERNEL_H_

#include <stdint.h>

void osKernelInit(void);
void osKernelLaunch(uint32_t quanta_ms);

//uint8_t osKernelAddThreads();
uint8_t osKernelAddThreads(void (*task0)(void),
                           void (*task1)(void),
                           void (*task2)(void));

void osThreadYield(void);
void osThreadSleep(uint32_t sleep_ms);

typedef enum {
    READY,
    BLOCKED_MUTEX,
    SLEEPING
} threadState_t;

void osThreadBlock(threadState_t reason);
void osThreadUnblock(int id);
int  osThreadGetId(void);



#endif
