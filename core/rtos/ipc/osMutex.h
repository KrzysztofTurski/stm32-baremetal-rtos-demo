#ifndef OS_MUTEX_H_
#define OS_MUTEX_H_

#include <stdint.h>

typedef struct {
    volatile uint8_t  locked;
    volatile int      owner;
    volatile uint32_t waitingMask;
} osMutex_t;

void osMutexInit(osMutex_t *m);
void osMutexLock(osMutex_t *m);
void osMutexUnlock(osMutex_t *m);

#endif
