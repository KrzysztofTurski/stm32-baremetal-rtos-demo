#ifndef USART2_H_
#define USART2_H_

#include <stdint.h>

/* Low-level blocking USART2 driver (TX only).
 * Must be called from a single context or protected by a mutex.
 */

void usart2_init_16mhz_115200(void);
void usart2_putc(char c);
void usart2_write(const char *s);

#endif
