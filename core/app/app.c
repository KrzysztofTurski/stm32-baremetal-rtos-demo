#include "osKernel.h"
#include <stdint.h>

#define QUANTA 10U

volatile uint32_t c0, c1, c2, hook;

void task3(void)
{
	hook++;
}

static void task0(void)
{
	while(1)
	{
		c0++;
		osThreadYield();
	}
}

static void task1(void)
{
	while(1)
	{
		c1++;
	}
}

static void task2(void)
{
	while(1)
	{
		c2++;
	}
}

void app_start(void)
{
	osKernelInit();
	osKernelAddThreads(&task0, &task1, &task2);
	osKernelLaunch(QUANTA);

	while(1) {}
}
