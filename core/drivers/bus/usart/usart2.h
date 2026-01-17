#pragma once
#include <stdint.h>

void usart2_init_16mhz_115200(void);
void usart2_putc(char c);
void usart2_write(const char *s);
