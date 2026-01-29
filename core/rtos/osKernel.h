#ifndef OSKERNEL_H_
#define OSKERNEL_H_

#include <stdint.h>

typedef enum {
    READY = 0,
    SLEEPING,
    BLOCKED_MUTEX,
    BLOCKED_QUEUE_RX,
    BLOCKED_QUEUE_TX
} threadState_t;

void    osKernelInit(void);
void    osKernelLaunch(uint32_t quanta_ms);

uint8_t osKernelAddThreads(void (*TaskLPS)(void),
                           void (*TaskSonar)(void),
                           void (*TaskLogger)(void));

void osThreadYield(void);
void osThreadSleep(uint32_t sleep_ms);

/* Contract:
 * - osThreadBlock() is called with IRQs already disabled by the caller.
 * - osThreadBlock() sets state, pends PendSV, then re-enables IRQs.
 * - osThreadUnblock() does NOT touch IRQ state (caller decides).
 */
void osThreadBlock(threadState_t reason);
void osThreadUnblock(int id);
int  osThreadGetId(void);

#endif
