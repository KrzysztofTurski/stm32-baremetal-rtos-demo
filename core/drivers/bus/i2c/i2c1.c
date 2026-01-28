#include "i2c1.h"

/*
    PB8 ---- SCL
    PB9 ---- SDA
*/

#define GPIOBEN                 (1U<<1)
#define I2C1EN                  (1U<<21)

#define I2C_100KHZ              80      // Standard mode 100kHz
#define SD_MODE_MAX_RISE_TIME   17
#define I2C_TIMEOUT 100000 




void I2C1_init(void)
{
    /* Enable clock access to GPIOB */
    RCC->AHB1ENR |= GPIOBEN;

    /* Set PB8 and PB9 mode to alternate function (AF4 for I2C1) */
    GPIOB->MODER &= ~(1U<<16);
    GPIOB->MODER |=  (1U<<17);
    GPIOB->MODER &= ~(1U<<18);
    GPIOB->MODER |=  (1U<<19);

    /* Set PB8 and PB9 output type to open-drain */
    GPIOB->OTYPER |= (1U<<8);
    GPIOB->OTYPER |= (1U<<9);

    /* Enable pull-up resistors for PB8 and PB9 */
    GPIOB->PUPDR &= ~(1U<<16);
    GPIOB->PUPDR |=  (1U<<17);
    GPIOB->PUPDR &= ~(1U<<18);
    GPIOB->PUPDR |=  (1U<<19);

    /* Set alternate function to AF4 (I2C1) */
    GPIOB->AFR[1] &= ~((0xF << (8-8)*4) | (0xF << (9-8)*4));
    GPIOB->AFR[1] |=  ((4   << (8-8)*4) | (4   << (9-8)*4));

    /* Enable clock access to I2C1 */
    RCC->APB1ENR |= I2C1EN;

    /* Reset I2C */
    I2C1->CR1 |= (1U<<15);
    I2C1->CR1 &= ~(1U<<15);

    /* Set peripheral clock frequency (16 MHz) */
    I2C1->CR2 = (1U<<4);

    /* Set I2C speed mode */
    I2C1->CCR = I2C_100KHZ;

    /* Set rise time */
    I2C1->TRISE = SD_MODE_MAX_RISE_TIME;

    /* Enable I2C peripheral */
    I2C1->CR1 |= CR1_PE;
}




/*--------------------------------------------------*/

void I2C1_burstWrite(char saddr, char maddr, int n, char* data)
{
    volatile int tmp;

    while (I2C1->SR2 & SR2_BUSY) {}

    I2C1->CR1 |= CR1_START;
    while (!(I2C1->SR1 & SR1_SB)) {}

    I2C1->DR = saddr << 1;
    while (!(I2C1->SR1 & SR1_ADDR)) {}

    tmp = I2C1->SR2;

    while (!(I2C1->SR1 & SR1_TXE)) {}
    I2C1->DR = maddr;

    while (n--) {
        while (!(I2C1->SR1 & SR1_TXE)) {}
        I2C1->DR = *data++;
    }

    while (!(I2C1->SR1 & SR1_BTF)) {}
    I2C1->CR1 |= CR1_STOP;
}



/*--------------------------------------------------*/


void I2C1_burstRead(char saddr, char maddr, int n, char* data)


{
    volatile int tmp;
    int timeout;

    timeout = I2C_TIMEOUT;
    while ((I2C1->SR2 & SR2_BUSY) && --timeout) {}
    if (timeout == 0) return;

    // START
    I2C1->CR1 |= CR1_START;
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & SR1_SB) && --timeout) {}
    if (timeout == 0) return;

  
    I2C1->DR = saddr << 1;
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & SR1_ADDR) && --timeout) {}
    if (timeout == 0) return;
    tmp = I2C1->SR2;


    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & SR1_TXE) && --timeout) {}
    if (timeout == 0) return;
    I2C1->DR = maddr;
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & SR1_TXE) && --timeout) {}
    if (timeout == 0) return;


    I2C1->CR1 |= CR1_START;
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & SR1_SB) && --timeout) {}
    if (timeout == 0) return;


    I2C1->DR = (saddr << 1) | 1;
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & SR1_ADDR) && --timeout) {}
    if (timeout == 0) return;
    tmp = I2C1->SR2;

    I2C1->CR1 |= CR1_ACK;


    while (n > 0U)
    {
        if (n == 1U)
        {
            I2C1->CR1 &= ~CR1_ACK;
            I2C1->CR1 |= CR1_STOP;

            timeout = I2C_TIMEOUT;
            while (!(I2C1->SR1 & SR1_RXNE) && --timeout) {}
            if (timeout == 0) return;
            *data++ = I2C1->DR;
            break;
        }
        else
        {
            timeout = I2C_TIMEOUT;
            while (!(I2C1->SR1 & SR1_RXNE) && --timeout) {}
            if (timeout == 0) return;
            *data++ = I2C1->DR;
        }
        n--;
    }
}

/*--------------------------------------------------*/

void I2C1_byteWrite(char saddr, char maddr, char data)
{
    while (I2C1->SR2 & SR2_BUSY);

    I2C1->CR1 |= CR1_START;
    while (!(I2C1->SR1 & SR1_SB));

    I2C1->DR = saddr << 1;
    while (!(I2C1->SR1 & SR1_ADDR));
    volatile int tmp = I2C1->SR2;

    while (!(I2C1->SR1 & SR1_TXE));
    I2C1->DR = maddr;

    while (!(I2C1->SR1 & SR1_TXE));
    I2C1->DR = data;

    while (!(I2C1->SR1 & SR1_BTF));
    I2C1->CR1 |= CR1_STOP;
}


/*--------------------------------------------------*/

void I2C1_byteRead(char saddr, char maddr, char* data)

{
    volatile int tmp;

    while (I2C1->SR2 & SR2_BUSY) {}

    I2C1->CR1 |= CR1_START;
    while (!(I2C1->SR1 & SR1_SB)) {}

    I2C1->DR = saddr << 1;
    while (!(I2C1->SR1 & SR1_ADDR)) {}
    tmp = I2C1->SR2;

    I2C1->DR = maddr;
    while (!(I2C1->SR1 & SR1_TXE)) {}

    I2C1->CR1 |= CR1_START;
    while (!(I2C1->SR1 & SR1_SB)) {}

    I2C1->DR = (saddr << 1) | 1;
    while (!(I2C1->SR1 & SR1_ADDR)) {}

    I2C1->CR1 &= ~CR1_ACK;
    tmp = I2C1->SR2;

    I2C1->CR1 |= CR1_STOP;
    while (!(I2C1->SR1 & SR1_RXNE)) {}

    *data++ = I2C1->DR;
}


/*--------------------------------------------------*/

