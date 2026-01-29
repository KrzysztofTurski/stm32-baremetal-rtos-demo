#include "osMutex.h"
#include "osKernel.h"
#include "stm32f4xx.h"
static int findFirst(uint32_t mask)
{
    for (int i = 0; i < 32; i++) {
        if (mask & (1U << i)) return i;
    }
    return -1;
}

void osMutexInit(osMutex_t *m)
{
    m->locked = 0;
    m->owner  = -1;
    m->waitingMask = 0;
}

void osMutexLock(osMutex_t *m)
{
    int me = osThreadGetId();

    __disable_irq();

    if (m->locked == 0) {
        m->locked = 1;
        m->owner  = me;
        __enable_irq();
        return;
    }

    /* Atomic wait: register as waiting while IRQs are OFF, then block.
       Prevents lost-wakeup when another task unlocks in the gap. */
    m->waitingMask |= (1U << me);
    osThreadBlock(BLOCKED_MUTEX);
}

void osMutexUnlock(osMutex_t *m)
{
    __disable_irq();

    if (m->owner != osThreadGetId()) {
        __enable_irq();
        return;
    }

    if (m->waitingMask != 0U) {
        int next = findFirst(m->waitingMask);
        m->waitingMask &= ~(1U << next);

        /* Direct handoff */
        m->locked = 1;
        m->owner  = next;

        osThreadUnblock(next);
    } else {
        m->locked = 0;
        m->owner  = -1;
    }

    __enable_irq();
}
