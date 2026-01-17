#pragma once
#include <stdint.h>

typedef struct {
    volatile uint8_t locked;
} osMutex_t;

void osMutexInit(osMutex_t *m);
void osMutexLock(osMutex_t *m);
void osMutexUnlock(osMutex_t *m);
