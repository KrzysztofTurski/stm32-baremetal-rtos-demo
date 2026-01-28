

#ifndef RTOS_IPC_OSMAILBOX_H_
#define RTOS_IPC_OSMAILBOX_H_
#pragma once
#include <stdint.h>

typedef struct {
    volatile uint32_t a;
    volatile uint32_t b;
    volatile uint8_t  hasData;        // 0 = brak, 1 = jest próbka
    volatile uint32_t waitingRxMask;  
} osMailbox2x32_t;

void osMailboxInit(osMailbox2x32_t *m);


void osMailboxPut(osMailbox2x32_t *m, uint32_t a, uint32_t b);


void osMailboxGet(osMailbox2x32_t *m, uint32_t *a, uint32_t *b);



#endif
