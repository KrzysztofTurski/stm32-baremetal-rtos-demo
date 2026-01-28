#include "osMailbox.h"
#include "osKernel.h"
#include "stm32f4xx.h"

/* identyczny pattern jak w osMutex.c */
static int findFirst(uint32_t mask)
{
    for (int i = 0; i < 32; i++) {
        if (mask & (1U << i)) return i;
    }
    return -1;
}

void osMailboxInit(osMailbox2x32_t *m)
{
    m->a = 0;
    m->b = 0;
    m->hasData = 0;
    m->waitingRxMask = 0;
}

/* Producer */
void osMailboxPut(osMailbox2x32_t *m, uint32_t a, uint32_t b)
{
    __disable_irq();

    
    m->a = a;
    m->b = b;

    
    m->hasData = 1;

    
    if (m->waitingRxMask != 0U) {
        int next = findFirst(m->waitingRxMask);
        if (next >= 0) {
            m->waitingRxMask &= ~(1U << next);
            osThreadUnblock(next);  // bez ruszania IRQ
        }
    }

    __enable_irq();
}

/* Consumer */
void osMailboxGet(osMailbox2x32_t *m, uint32_t *a, uint32_t *b)
{
    while (1)
    {
        __disable_irq();

        
        if (m->hasData)
        {
            uint32_t aa = m->a;
            uint32_t bb = m->b;

            m->hasData = 0;

            __enable_irq();

            if (a) *a = aa;
            if (b) *b = bb;
            return;
        }

        
        int me = osThreadGetId();
        m->waitingRxMask |= (1U << me);

        
        osThreadBlock(BLOCKED_QUEUE_RX);

        
    }
}
