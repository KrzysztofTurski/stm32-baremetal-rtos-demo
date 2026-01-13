#include "osKernel.h"

#define NUM_OF_THREADS        3
#define STACKSIZE             800
#define BUS_FREQ              16000000

#define PENDSVSET             (1U << 28)
//#define PENDSTSET             (1U << 26)

#define INTCTRL               (*((volatile uint32_t *)0xE000ED04))

#define CTRL_ENABLE           (1U << 0)
#define CTRL_TICKINT          (1U << 1)
#define CTRL_CLCKSRC          (1U << 2)
//#define CTRL_COUNTFLAG        (1U << 16)
#define SYSTICK_RST           0

//#define TIM2EN                (1U << 0)
//#define CR1_CEN               (1U << 0)
//#define DIER_UIE              (1U << 0)

volatile uint32_t period_tick;
uint32_t MILLIS_PRESCALER;

void osSchedulerLaunch(void);
void osSchedulerRoundRobin(void);

struct tcb {
    int32_t *stackPt;
    struct tcb *nextPt;
};

typedef struct tcb tcbType;

tcbType tcbs[NUM_OF_THREADS];
tcbType *currentPt;

int32_t TCB_STACK[NUM_OF_THREADS][STACKSIZE];

void osKernelStackInit(int i)
{
    tcbs[i].stackPt = &TCB_STACK[i][STACKSIZE - 16];

    TCB_STACK[i][STACKSIZE - 1]  = (1U << 24);
    TCB_STACK[i][STACKSIZE - 3]  = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 4]  = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 5]  = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 6]  = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 7]  = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 8]  = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 9]  = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 10] = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 11] = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 12] = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 13] = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 14] = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 15] = 0xAAAAAAAA;
    TCB_STACK[i][STACKSIZE - 16] = 0xAAAAAAAA;
}

uint8_t osKernelAddThreads(void (*task0)(void),
                           void (*task1)(void),
                           void (*task2)(void))
{
    __disable_irq();

    tcbs[0].nextPt = &tcbs[1];
    tcbs[1].nextPt = &tcbs[2];
    tcbs[2].nextPt = &tcbs[0];

    osKernelStackInit(0);
    TCB_STACK[0][STACKSIZE - 2] = (int32_t)(task0);

    osKernelStackInit(1);
    TCB_STACK[1][STACKSIZE - 2] = (int32_t)(task1);

    osKernelStackInit(2);
    TCB_STACK[2][STACKSIZE - 2] = (int32_t)(task2);

    currentPt = &tcbs[0];

    __enable_irq();
    return 1;
}

void osKernelInit(void)
{
    MILLIS_PRESCALER = (BUS_FREQ / 1000);
}

void osKernelLaunch(uint32_t quanta)
{
    SysTick->CTRL = SYSTICK_RST;
    SysTick->VAL  = 0;
    SysTick->LOAD = (quanta * MILLIS_PRESCALER) - 1;

    NVIC_SetPriority(SysTick_IRQn, 14);
    NVIC_SetPriority(PendSV_IRQn, 15);

    SysTick->CTRL = CTRL_CLCKSRC | CTRL_ENABLE;
    SysTick->CTRL |= CTRL_TICKINT;

    osSchedulerLaunch();
}

void SysTick_Handler(void)
{
    INTCTRL = PENDSVSET;
}

void osThreadYield(void)
{
    INTCTRL = PENDSVSET;
}

void osSchedulerRoundRobin(void)
{
    if ((++period_tick) == PERIOD) {
        task3();
        period_tick = 0;
    }

    currentPt = currentPt->nextPt;
}

/*void tim2_1hz_interrupt_init(void)
{
    RCC->APB1ENR |= TIM2EN;
    TIM2->PSC = 1600 - 1;
    TIM2->ARR = 10000 - 1;
    TIM2->CNT = 0;
    TIM2->CR1 = CR1_CEN;
    TIM2->DIER |= DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);
}
*/
