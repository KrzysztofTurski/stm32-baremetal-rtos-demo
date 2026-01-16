#include "app.h"
#include "osKernel.h"
#include <stdint.h>

#define QUANTA_MS 1

volatile uint32_t count0, count1, count2;

static void Task0(void){ while(1){ count0++; osThreadYield(); } }
static void Task1(void){ while(1){ count1++; osThreadSleep(250); } }
static void Task2(void){ while(1){ count2++; osThreadSleep(500); } }

void app_start(void)
{
    osKernelInit();
    osKernelAddThreads(&Task0, &Task1, &Task2);
    osKernelLaunch(QUANTA_MS);
    while(1){}
}
