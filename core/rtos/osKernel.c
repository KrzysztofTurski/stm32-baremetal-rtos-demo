#include "osKernel.h"
#include "stm32f4xx.h"

static void IdleTask(void);

#define NUM_OF_THREADS    4
#define IDLE_THREAD_IDX  3
#define NUM_THREADS      4

#define STACKSIZE        800
#define BUS_FREQ         16000000U

#define CTRL_ENABLE      (1U << 0)
#define CTRL_TICKINT     (1U << 1)
#define CTRL_CLKSRC      (1U << 2)

#define PENDSVSET        (1U << 28)

uint32_t MILLIS_PRESCALER;
typedef enum {
    READY,
    BLOCKED_MUTEX,
    SLEEPING
} threadState_t;


typedef struct tcb {
    int32_t *stackPt;
    struct tcb *nextPt;

    volatile uint32_t sleepTime;
    threadState_t state;
} tcbType;


tcbType tcbs[NUM_OF_THREADS];
tcbType *currentPt;

static int32_t TCB_STACK[NUM_OF_THREADS][STACKSIZE];

void osSchedulerLaunch(void);
void osSchedulerRRWithSleep(void);
/*--------------------------------------------------*/

static void IdleTask(void)
{
    while (1) {
        __WFI();
    }
}

/*--------------------------------------------------*/


static void osKernelStackInit(int i)
{
    tcbs[i].stackPt = &TCB_STACK[i][STACKSIZE - 16];

    TCB_STACK[i][STACKSIZE - 1]  = 0x01000000;
    TCB_STACK[i][STACKSIZE - 3]  = 0x14141414;
    TCB_STACK[i][STACKSIZE - 4]  = 0x12121212;
    TCB_STACK[i][STACKSIZE - 5]  = 0x03030303;
    TCB_STACK[i][STACKSIZE - 6]  = 0x02020202;
    TCB_STACK[i][STACKSIZE - 7]  = 0x01010101;
    TCB_STACK[i][STACKSIZE - 8]  = 0x00000000;
    TCB_STACK[i][STACKSIZE - 9]  = 0x11111111;
    TCB_STACK[i][STACKSIZE - 10] = 0x10101010;
    TCB_STACK[i][STACKSIZE - 11] = 0x09090909;
    TCB_STACK[i][STACKSIZE - 12] = 0x08080808;
    TCB_STACK[i][STACKSIZE - 13] = 0x07070707;
    TCB_STACK[i][STACKSIZE - 14] = 0x06060606;
    TCB_STACK[i][STACKSIZE - 15] = 0x05050505;
    TCB_STACK[i][STACKSIZE - 16] = 0x04040404;
}

/*--------------------------------------------------*/

uint8_t osKernelAddThreads(void (*task0)(void),
                           void (*task1)(void),
                           void (*task2)(void))
{
    __disable_irq();

    tcbs[0].nextPt = &tcbs[1];
    tcbs[1].nextPt = &tcbs[2];
    tcbs[2].nextPt = &tcbs[IDLE_THREAD_IDX];
    tcbs[IDLE_THREAD_IDX].nextPt = &tcbs[0];

    osKernelStackInit(0);
    TCB_STACK[0][STACKSIZE - 2] = (int32_t)task0;

    osKernelStackInit(1);
    TCB_STACK[1][STACKSIZE - 2] = (int32_t)task1;

    osKernelStackInit(2);
    TCB_STACK[2][STACKSIZE - 2] = (int32_t)task2;

    osKernelStackInit(IDLE_THREAD_IDX);
    TCB_STACK[IDLE_THREAD_IDX][STACKSIZE - 2] = (int32_t)IdleTask;

    for (int i = 0; i < NUM_OF_THREADS; i++) {
            tcbs[i].sleepTime = 0;
            tcbs[i].state = READY;
    }
    currentPt = &tcbs[0];

    __enable_irq();
    return 1;
}

/*--------------------------------------------------*/

void osKernelInit(void)
{
    MILLIS_PRESCALER = (BUS_FREQ / 1000U);
}

/*--------------------------------------------------*/

void osKernelLaunch(uint32_t quanta_ms)
{
    SysTick->CTRL = 0;
    SysTick->VAL  = 0;
    SysTick->LOAD = (quanta_ms * MILLIS_PRESCALER) - 1U;

    NVIC_SetPriority(SysTick_IRQn, 14);
    NVIC_SetPriority(PendSV_IRQn, 15);

    SysTick->CTRL = CTRL_CLKSRC | CTRL_TICKINT | CTRL_ENABLE;

    osSchedulerLaunch();
}

/*--------------------------------------------------*/

void osThreadYield(void)
{
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/*--------------------------------------------------*/

void osThreadSleep(uint32_t sleep_ms)
{
    __disable_irq();
    currentPt->sleepTime = sleep_ms;
    currentPt->state = SLEEPING;
    __enable_irq();

    osThreadYield();
}

/*--------------------------------------------------*/

void SysTick_Handler(void)
{
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        if (tcbs[i].sleepTime > 0)
        {
            tcbs[i].sleepTime--;

            if (tcbs[i].sleepTime == 0 &&
                tcbs[i].state == SLEEPING)
            {
                tcbs[i].state = READY;
            }
        }
    }

    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/*--------------------------------------------------*/

void osSchedulerRRWithSleep(void)
{
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        currentPt = currentPt->nextPt;

        if (currentPt->state == BLOCKED_MUTEX)
            continue;

        if (currentPt->state == SLEEPING)
            continue;

        return;
    }

    currentPt = &tcbs[IDLE_THREAD_IDX];
}

/*--------------------------------------------------*/
int osThreadGetId(void)
{
    return (int)(currentPt - &tcbs[0]);
}
/*--------------------------------------------------*/
void osThreadBlock(void)
{
    __disable_irq();
    currentPt->state = BLOCKED_MUTEX;
    __enable_irq();

    osThreadYield();
}

/*--------------------------------------------------*/

void osThreadUnblock(int id)
{
    __disable_irq();
    tcbs[id].state = READY;
    __enable_irq();
}
/*--------------------------------------------------*/
