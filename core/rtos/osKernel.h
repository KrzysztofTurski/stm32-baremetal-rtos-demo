
#ifndef OSKERNEL_H_
#define OSKERNEL_H_

#include <stdint.h>

void osKernelInit(void);
void osKernelLaunch(uint32_t quanta_ms);


uint8_t osKernelAddThreads(void (*TaskLPS)(void),
		   	   	   	   	   void (*TaskLogger)(void),
						   void (*task2)(void));


void osThreadYield(void);
void osThreadSleep(uint32_t sleep_ms);

typedef enum {
    READY = 0,
    SLEEPING,
    BLOCKED_MUTEX,
    BLOCKED_QUEUE_RX,
    BLOCKED_QUEUE_TX
} threadState_t;


void osThreadBlock(threadState_t reason);
void osThreadUnblock(int id);
int  osThreadGetId(void);



#endif
