#include "log.h"
#include "app.h"

#include "osKernel.h"
#include "usart2.h"
#include <stdint.h>

#define QUANTA_MS 10

volatile uint32_t count0, count1, count2;

static void Task0(void){ while(1){  count0++; 
									//osThreadSleep(500);


									log_write("testmutex0\n");
									osThreadYield();
									}}
static void Task1(void){ while(1){  count1++;
									osThreadSleep(500);
									log_write("testmutex1\n"); }}
static void Task2(void){ while(1){ count2++;
								    osThreadSleep(500);
								    log_write("testmutex2\n"); }}
static void TaskLog(void)
{
    while (1) {
    	log_write ("RTOS alive\n");
        osThreadSleep(500);
    }
}

void app_start(void)
{

	usart2_init_16mhz_115200();
	log_init();
	log_write("USART2 OK\n");
	

	osKernelInit();
    osKernelAddThreads(&Task0, &Task1, &Task2);
    osKernelLaunch(QUANTA_MS);
    while(1){}
}
