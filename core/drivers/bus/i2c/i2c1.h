
#ifndef I2C1_H_
#define I2C1_H_

#include <stdint.h>
#include "stm32f4xx.h"

void I2C1_init(void);

void I2C1_byteRead(char saddr, char maddr, char *data);
void I2C1_byteWrite(char saddr, char maddr, char data);

void I2C1_burstRead(char saddr, char maddr, int n, char *data);
void I2C1_burstWrite(char saddr, char maddr, int n, char *data);

/* SR1 bits */
#define SR1_SB      (1U<<0)
#define SR1_ADDR    (1U<<1)
#define SR1_BTF     (1U<<2)
#define SR1_RXNE    (1U<<6)
#define SR1_TXE     (1U<<7)

/* SR2 bits */
#define SR2_BUSY    (1U<<1)

/* CR1 bits */
#define CR1_PE      (1U<<0)
#define CR1_START   (1U<<8)
#define CR1_STOP    (1U<<9)
#define CR1_ACK     (1U<<10)

#endif 
