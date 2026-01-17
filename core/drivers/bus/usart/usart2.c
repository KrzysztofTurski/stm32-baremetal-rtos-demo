#include "usart2.h"
#include "stm32f4xx.h"

#define GPIOAEN     (1U << 0)
#define USART2_EN   (1U << 17)

static void usart2_set_brr_overs16(uint32_t pclk_hz, uint32_t baud)
{
    
    uint32_t brr = (pclk_hz + (baud / 2U)) / baud;
    USART2->BRR = brr;
}

void usart2_init_16mhz_115200(void)
{
    // 1) GPIOA clock
    RCC->AHB1ENR |= GPIOAEN;

    // 2) PA2 = Alternate Function (10b)
    GPIOA->MODER &= ~(3U << (2U * 2U));   // clear MODER2
    GPIOA->MODER |=  (2U << (2U * 2U));   // set AF

    // 3) AF7 on PA2 (USART2_TX)
    GPIOA->AFR[0] &= ~(0xFU << (4U * 2U)); // clear AFRL2
    GPIOA->AFR[0] |=  (7U   << (4U * 2U)); // AF7

    

    // 4) USART2 clock
    RCC->APB1ENR |= USART2_EN;

    // 5) Disable before config
    USART2->CR1 = 0;

    // 6) Baudrate (zakładamy PCLK1 = 16 MHz)
    usart2_set_brr_overs16(16000000U, 115200U);

    // 7) Enable TX + USART
    USART2->CR1 |= (1U << 3);   // TE
    USART2->CR1 |= (1U << 13);  // UE
}

void usart2_putc(char c)
{
    while (!(USART2->SR & USART_SR_TXE)) { }
    USART2->DR = (uint8_t)c;
}

void usart2_write(const char *s)
{
    while (*s) {
        if (*s == '\n') { usart2_putc('\r'); }
        usart2_putc(*s++);
    }
}
