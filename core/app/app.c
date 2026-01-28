#include <stdio.h>
#include <stdint.h>

#include "log.h"
#include "app.h"
#include "osKernel.h"
#include "usart2.h"
#include "i2c1.h"
#include "lps25hb.h"
#include "osMailbox.h"
/*--------------------------------------------------*/
#define QUANTA_MS 10
/*--------------------------------------------------*/
static osMailbox2x32_t lpsMbox;
volatile uint32_t count0, count1, count2;

/*--------------------------------------------------*/

static void TaskLPS(void)
{
    while (1)
    {
        int16_t  temp_raw  = LPS25HB_readTemperature();
        uint32_t press_raw = LPS25HB_readPressure();

        
        osMailboxPut(&lpsMbox, (uint32_t)(uint16_t)temp_raw, press_raw);

        osThreadSleep(1000);
    }
}
/*--------------------------------------------------*/

static void TaskLogger(void)
{
    while (1)
    {
        uint32_t temp_u16;
        uint32_t press_raw;

        osMailboxGet(&lpsMbox, &temp_u16, &press_raw);

        int16_t temp_raw = (int16_t)(uint16_t)temp_u16;

        
        int32_t temp_scaled = 4250 + ((int32_t)temp_raw * 100) / 480;
        int t_int  = temp_scaled / 100;
        int t_frac = temp_scaled % 100;

        int32_t press_scaled = ((int32_t)press_raw * 100) / 4096;
        int p_int  = press_scaled / 100;
        int p_frac = press_scaled % 100;

        char buf[64];
        snprintf(buf, sizeof(buf),
                 "LPS: %d.%02d C | %d.%02d hPa\n",
                 t_int, t_frac, p_int, p_frac);

        log_write(buf);
    }
}






/*--------------------------------------------------*/


/*--------------------------------------------------*/

static void Task2(void){ while(1){ count2++;
								    osThreadSleep(500);
								    log_write("testmutex2\n"); }}




/*--------------------------------------------------*/

void app_start(void)
{

	usart2_init_16mhz_115200();
	log_init();
	log_write("USART2 OK\n");
	
	I2C1_init();
    LPS25HB_init();

    uint8_t who = LPS25HB_readWhoAmI();
    if (who == LPS25HB_WHOAMI_EXPECTED) {
        log_write("LPS25HB_OK\n");
    } else {
        log_write("LPS25HB_ERROR\n");
    }
	
	
    osMailboxInit(&lpsMbox);

	osKernelInit();
    osKernelAddThreads(&TaskLPS,&TaskLogger , &Task2);

    osKernelLaunch(QUANTA_MS);
    while(1){}
}
/*--------------------------------------------------*/

//static void TaskLog(void)
//{
 //   while (1) {
  //  	log_write ("RTOS alive\n");
   //     osThreadSleep(500);
   // }
//}
/*--------------------------------------------------*/

//static void Task0(void){ while(1){  count0++;
									//osThreadSleep(500);


								//	log_write("testmutex0\n");
								//	osThreadYield();



//	}}
//static void Task1(void){ while(1){  count1++;
								//	osThreadSleep(1000);
								//	log_write("testmutex1\n"); }}
