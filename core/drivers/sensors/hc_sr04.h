#ifndef HC_SR04_H_
#define HC_SR04_H_

#include <stdint.h>

void     HC_init(void);
uint32_t measure_distance_cm(void);


void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

#endif
