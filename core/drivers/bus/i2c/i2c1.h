
#ifndef I2C_H_
#define I2C_H_

#include "stm32f4xx.h"
#include <stdint.h>



void I2C1_init(void);
void I2C1_byteRead(char saddr, char maddr, char* data);
void I2C1_byteWrite(char saddr, char maddr, char data);
void I2C1_burstRead(char saddr, char maddr, int n, char* data);
void I2C1_burstWrite(char saddr, char maddr, int n, char* data);


#define SR1_SB      (1U<<0)
#define SR1_ADDR    (1U<<1)
#define SR1_BTF     (1U<<2)
#define SR1_RXNE    (1U<<6)
#define SR1_TXE     (1U<<7)

#define SR2_BUSY    (1U<<1)

#define CR1_START   (1U<<8)
#define CR1_STOP    (1U<<9)
#define CR1_ACK     (1U<<10)
#define CR1_PE      (1U<<0)
#define LPS25HB_ADDR 0x5C




#endif 
