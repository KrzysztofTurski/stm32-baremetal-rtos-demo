
#include "stm32f4xx.h"
#include <stdint.h>


#define LPS25HB_WHOAMI_EXPECTED  0xBD

void LPS25HB_init(void);
uint32_t LPS25HB_readPressure(void);
int16_t LPS25HB_readTemperature(void);
uint8_t LPS25HB_readWhoAmI(void);
