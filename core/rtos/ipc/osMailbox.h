

#ifndef RTOS_IPC_OSMAILBOX_H_
#define RTOS_IPC_OSMAILBOX_H_
#pragma once
#include <stdint.h>

typedef struct {
    volatile uint32_t a;
    volatile uint32_t b;
    volatile uint8_t  hasData;        // 0 = brak, 1 = jest próbka
    volatile uint32_t waitingRxMask;  // bity tasków czekających na RX
} osMailbox2x32_t;

void osMailboxInit(osMailbox2x32_t *m);

/* Producer: zapisuje "ostatnią próbkę", nadpisuje poprzednią, budzi jednego RX */
void osMailboxPut(osMailbox2x32_t *m, uint32_t a, uint32_t b);

/* Consumer: jeśli brak danych → blokuje task w BLOCKED_QUEUE_RX aż producer coś wrzuci */
void osMailboxGet(osMailbox2x32_t *m, uint32_t *a, uint32_t *b);



#endif
