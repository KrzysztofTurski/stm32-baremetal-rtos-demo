#include "osMutex.h"
#include "osKernel.h"
#include "stm32f4xx.h"

void osMutexInit(osMutex_t *m)
{
    m->locked = 0;
}

void osMutexLock(osMutex_t *m)
{
    for (;;) {
        __disable_irq();
        if (m->locked == 0) {
            m->locked = 1;
            __enable_irq();
            return;
        }
        __enable_irq();


        osThreadYield();
    }
}

void osMutexUnlock(osMutex_t *m)
{
    __disable_irq();
    m->locked = 0;
    __enable_irq();
}
