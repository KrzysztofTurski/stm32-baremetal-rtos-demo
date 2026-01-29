#include "i2c1.h"

/* I2C1 pins:
 * PB8 = SCL (AF4), PB9 = SDA (AF4)
 * Assumes APB1 = 16 MHz for timing below.
 */

#define GPIOBEN               (1U<<1)
#define I2C1EN                (1U<<21)

#define I2C_100KHZ            80U
#define SD_MODE_MAX_RISE_TIME 17U
#define I2C_TIMEOUT           100000U

void I2C1_init(void)
{
    RCC->AHB1ENR |= GPIOBEN;

    /* PB8, PB9 -> AF */
    GPIOB->MODER &= ~(3U<<(8*2));
    GPIOB->MODER |=  (2U<<(8*2));
    GPIOB->MODER &= ~(3U<<(9*2));
    GPIOB->MODER |=  (2U<<(9*2));

    /* open-drain */
    GPIOB->OTYPER |= (1U<<8) | (1U<<9);

    /* pull-up */
    GPIOB->PUPDR &= ~(3U<<(8*2));
    GPIOB->PUPDR |=  (1U<<(8*2));
    GPIOB->PUPDR &= ~(3U<<(9*2));
    GPIOB->PUPDR |=  (1U<<(9*2));

    /* AF4 */
    GPIOB->AFR[1] &= ~((0xFU << ((8-8)*4)) | (0xFU << ((9-8)*4)));
    GPIOB->AFR[1] |=  ((4U   << ((8-8)*4)) | (4U   << ((9-8)*4)));

    RCC->APB1ENR |= I2C1EN;

    /* reset I2C */
    I2C1->CR1 |= (1U<<15);
    I2C1->CR1 &= ~(1U<<15);

    /* PCLK1 freq = 16 MHz */
    I2C1->CR2 = (1U<<4);

    I2C1->CCR   = I2C_100KHZ;
    I2C1->TRISE = SD_MODE_MAX_RISE_TIME;

    I2C1->CR1 |= CR1_PE;
}

void I2C1_burstWrite(char saddr, char maddr, int n, char *data)
{
    volatile int tmp;

    while (I2C1->SR2 & SR2_BUSY) {}

    I2C1->CR1 |= CR1_START;
    while (!(I2C1->SR1 & SR1_SB)) {}

    I2C1->DR = (uint8_t)(saddr << 1);
    while (!(I2C1->SR1 & SR1_ADDR)) {}
    tmp = I2C1->SR2;

    while (!(I2C1->SR1 & SR1_TXE)) {}
    I2C1->DR = (uint8_t)maddr;

    while (n--) {
        while (!(I2C1->SR1 & SR1_TXE)) {}
        I2C1->DR = (uint8_t)(*data++);
    }

    while (!(I2C1->SR1 & SR1_BTF)) {}
    I2C1->CR1 |= CR1_STOP;
}

void I2C1_burstRead(char saddr, char maddr, int n, char *data)
{
    volatile int tmp;
    uint32_t timeout;

    timeout = I2C_TIMEOUT;
    while ((I2C1->SR2 & SR2_BUSY) && --timeout) {}
    if (timeout == 0U) return;

    I2C1->CR1 |= CR1_START;
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & SR1_SB) && --timeout) {}
    if (timeout == 0U) return;

    I2C1->DR = (uint8_t)(saddr << 1);
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & SR1_ADDR) && --timeout) {}
    if (timeout == 0U) return;
    tmp = I2C1->SR2;

    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & SR1_TXE) && --timeout) {}
    if (timeout == 0U) return;
    I2C1->DR = (uint8_t)maddr;

    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & SR1_TXE) && --timeout) {}
    if (timeout == 0U) return;

    I2C1->CR1 |= CR1_START;
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & SR1_SB) && --timeout) {}
    if (timeout == 0U) return;

    I2C1->DR = (uint8_t)((saddr << 1) | 1U);
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & SR1_ADDR) && --timeout) {}
    if (timeout == 0U) return;
    tmp = I2C1->SR2;

    I2C1->CR1 |= CR1_ACK;

    while (n > 0) {
        if (n == 1) {
            I2C1->CR1 &= ~CR1_ACK;
            I2C1->CR1 |= CR1_STOP;

            timeout = I2C_TIMEOUT;
            while (!(I2C1->SR1 & SR1_RXNE) && --timeout) {}
            if (timeout == 0U) return;

            *data++ = (char)I2C1->DR;
            break;
        } else {
            timeout = I2C_TIMEOUT;
            while (!(I2C1->SR1 & SR1_RXNE) && --timeout) {}
            if (timeout == 0U) return;

            *data++ = (char)I2C1->DR;
        }
        n--;
    }
}

void I2C1_byteWrite(char saddr, char maddr, char data)
{
    volatile int tmp;

    while (I2C1->SR2 & SR2_BUSY) {}

    I2C1->CR1 |= CR1_START;
    while (!(I2C1->SR1 & SR1_SB)) {}

    I2C1->DR = (uint8_t)(saddr << 1);
    while (!(I2C1->SR1 & SR1_ADDR)) {}
    tmp = I2C1->SR2;

    while (!(I2C1->SR1 & SR1_TXE)) {}
    I2C1->DR = (uint8_t)maddr;

    while (!(I2C1->SR1 & SR1_TXE)) {}
    I2C1->DR = (uint8_t)data;

    while (!(I2C1->SR1 & SR1_BTF)) {}
    I2C1->CR1 |= CR1_STOP;
}

void I2C1_byteRead(char saddr, char maddr, char *data)
{
    volatile int tmp;

    while (I2C1->SR2 & SR2_BUSY) {}

    I2C1->CR1 |= CR1_START;
    while (!(I2C1->SR1 & SR1_SB)) {}

    I2C1->DR = (uint8_t)(saddr << 1);
    while (!(I2C1->SR1 & SR1_ADDR)) {}
    tmp = I2C1->SR2;

    while (!(I2C1->SR1 & SR1_TXE)) {}
    I2C1->DR = (uint8_t)maddr;

    I2C1->CR1 |= CR1_START;
    while (!(I2C1->SR1 & SR1_SB)) {}

    I2C1->DR = (uint8_t)((saddr << 1) | 1U);
    while (!(I2C1->SR1 & SR1_ADDR)) {}

    I2C1->CR1 &= ~CR1_ACK;
    tmp = I2C1->SR2;

    I2C1->CR1 |= CR1_STOP;
    while (!(I2C1->SR1 & SR1_RXNE)) {}

    *data = (char)I2C1->DR;
}
