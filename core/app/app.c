#include <stdio.h>
#include <stdint.h>

#include "app.h"
#include "log.h"

#include "osKernel.h"
#include "osMailbox.h"

#include "usart2.h"
#include "i2c1.h"
#include "lps25hb.h"
#include "hc_sr04.h"

#define QUANTA_MS 10

static osMailbox2x32_t lpsMbox;
static osMailbox2x32_t sonarMbox;

static inline uint32_t median3(uint32_t a, uint32_t b, uint32_t c)
{
    if (a > b) { uint32_t t = a; a = b; b = t; }
    if (b > c) { uint32_t t = b; b = c; c = t; }
    if (a > b) { uint32_t t = a; a = b; b = t; }
    return b;
}

/* Producer: reads LPS raw values and publishes latest sample */
static void TaskLPS(void)
{
    while (1) {
        int16_t  temp_raw  = LPS25HB_readTemperature();
        uint32_t press_raw = LPS25HB_readPressure();

        /* Pipeline: push RAW to mailbox; formatting/logging is done by TaskLogger */
        osMailboxPut(&lpsMbox, (uint32_t)(uint16_t)temp_raw, press_raw);

        osThreadSleep(1000);
    }
}

/* Producer: reads HC-SR04 distance and publishes filtered (median) sample */
static void TaskSonar(void)
{
    while (1) {
        uint32_t d1 = measure_distance_cm();
        osThreadSleep(20);

        uint32_t d2 = measure_distance_cm();
        osThreadSleep(20);

        uint32_t d3 = measure_distance_cm();

        uint32_t d_med = median3(d1, d2, d3);
        osMailboxPut(&sonarMbox, d_med, 0);

        osThreadSleep(60);
    }
}

/* Consumer: single UART/logging task */
static void TaskLogger(void)
{
    while (1) {
        uint32_t a, b;

        if (osMailboxTryGet(&sonarMbox, &a, &b)) {
            char buf[48];
            if (a == 0xFFFFU) {
                snprintf(buf, sizeof(buf), "HC: Timeout/error\n");
            } else {
                snprintf(buf, sizeof(buf), "HC: D=%lu cm\n", a);
            }
            log_write(buf);
        }

        if (osMailboxTryGet(&lpsMbox, &a, &b)) {
            int16_t  temp_raw  = (int16_t)(uint16_t)a;
            uint32_t press_raw = b;

            int32_t temp_scaled = 4250 + ((int32_t)temp_raw * 100) / 480;
            int t_int  = temp_scaled / 100;
            int t_frac = temp_scaled % 100;

            int32_t press_scaled = ((int32_t)press_raw * 100) / 4096;
            int p_int  = press_scaled / 100;
            int p_frac = press_scaled % 100;

            char buf[64];
            snprintf(buf, sizeof(buf),
                     "LPS : %d.%02d C | %d.%02d hPa\n",
                     t_int, t_frac, p_int, p_frac);
            log_write(buf);
        }

        osThreadSleep(200);
    }
}

void app_start(void)
{
    usart2_init_16mhz_115200();
    log_init();
    log_write("USART2 OK\n");

    I2C1_init();
    LPS25HB_init();

    HC_init();
    log_write("HC_SR04 OK\n");

    uint8_t who = LPS25HB_readWhoAmI();
    log_write((who == LPS25HB_WHOAMI_EXPECTED) ? "LPS25HB_OK\n" : "LPS25HB_ERROR\n");

    osMailboxInit(&lpsMbox);
    osMailboxInit(&sonarMbox);

    osKernelInit();
    osKernelAddThreads(&TaskLPS, &TaskSonar, &TaskLogger);
    osKernelLaunch(QUANTA_MS);

    while (1) {}
}
