#ifndef LPS25HB_H
#define LPS25HB_H

#include <stdint.h>
#include "stm32f4xx.h"

/*
 * LPS25HB – czujnik ciśnienia i temperatury
 */

#define LPS25HB_WHOAMI_EXPECTED  0xBD

/* Inicjalizacja czujnika */
void LPS25HB_init(void);

/* Odczyt ciśnienia (surowa wartość lub przeliczona – zależnie od implementacji) */
uint32_t LPS25HB_readPressure(void);

/* Odczyt temperatury */
int16_t LPS25HB_readTemperature(void);

/* Odczyt rejestru WHO_AM_I */
uint8_t LPS25HB_readWhoAmI(void);

#endif /* LPS25HB_H */




