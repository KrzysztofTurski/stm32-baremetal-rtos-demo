#ifndef RTOS_IPC_OSMAILBOX_H_
#define RTOS_IPC_OSMAILBOX_H_
#include "stm32f4xx.h"
#include <stdint.h>

/* Mailbox "latest" (single-slot):
 * - osMailboxPut() overwrites previous sample
 * - osMailboxGet() blocks caller until data becomes available
 */
typedef struct {
    volatile uint32_t a;
    volatile uint32_t b;
    volatile uint8_t  hasData;
    volatile uint32_t waitingRxMask;
} osMailbox2x32_t;

void osMailboxInit(osMailbox2x32_t *m);

void osMailboxPut(osMailbox2x32_t *m, uint32_t a, uint32_t b);

void osMailboxGet(osMailbox2x32_t *m, uint32_t *a, uint32_t *b);

/* Non-blocking read; returns 1 if data read, 0 if mailbox empty */
int  osMailboxTryGet(osMailbox2x32_t *m, uint32_t *a, uint32_t *b);

#endif /* RTOS_IPC_OSMAILBOX_H_ */
