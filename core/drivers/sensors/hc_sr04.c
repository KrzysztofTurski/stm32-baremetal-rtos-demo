#include "stm32f4xx.h"
#include <stdint.h>
#include "hc_sr04.h"

void delay_us(uint32_t us)
{
    volatile uint32_t count = us * 6;
    while (count--) __asm__("nop");
}

void delay_ms(uint32_t ms)
{
    while (ms--) delay_us(1000);
}

void HC_init(void)
{
    /* clocks */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* PA0 = TRIG output */
    GPIOA->MODER &= ~(3U << (0 * 2));
    GPIOA->MODER |=  (1U << (0 * 2));
    GPIOA->OTYPER &= ~(1U << 0);

    /* PA1 = ECHO AF1 (TIM2_CH2) */
    GPIOA->MODER &= ~(3U << (1 * 2));
    GPIOA->MODER |=  (2U << (1 * 2));          // AF
    GPIOA->AFR[0] &= ~(0xFU << (1 * 4));
    GPIOA->AFR[0] |=  (1U   << (1 * 4));       // AF1

    /* TIM2: 1 us tick @ 16MHz */
    TIM2->PSC = 15U;
    TIM2->ARR = 0xFFFFU;

    /* CH2 input capture mapped to TI2:
       CC2S=01 (bits 9:8 in CCMR1) */
    TIM2->CCMR1 &= ~(TIM_CCMR1_CC2S_Msk | TIM_CCMR1_IC2F_Msk | TIM_CCMR1_IC2PSC_Msk);
    TIM2->CCMR1 |=  (1U << TIM_CCMR1_CC2S_Pos);

    /* enable capture on CH2, rising edge by default */
    TIM2->CCER &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
    TIM2->CCER |=  TIM_CCER_CC2E;

    /* start timer */
    TIM2->CR1 |= TIM_CR1_CEN;

    delay_ms(100);
}

uint32_t measure_distance_cm(void)
{
    uint32_t start_time, end_time, pulse_width;
    uint32_t timeout;

    /* clear CC2IF, reset counter */
    TIM2->SR  &= ~TIM_SR_CC2IF;
    TIM2->CNT  = 0;

    /* rising edge */
    TIM2->CCER &= ~TIM_CCER_CC2P;

    /* TRIG 10us */
    GPIOA->ODR |= (1U << 0);
    delay_us(10);
    GPIOA->ODR &= ~(1U << 0);
    delay_us(100);

    /* wait rising capture */
    timeout = 30000U;
    while (((TIM2->SR & TIM_SR_CC2IF) == 0U) && timeout--) {
        if (TIM2->CNT > 60000U) return 0xFFFFU;
    }
    if (timeout == 0U) return 0xFFFFU;

    start_time = TIM2->CCR2;
    TIM2->SR &= ~TIM_SR_CC2IF;

    /* falling edge */
    TIM2->CCER |= TIM_CCER_CC2P;

    /* wait falling capture */
    timeout = 30000U;
    while (((TIM2->SR & TIM_SR_CC2IF) == 0U) && timeout--) {
        if (TIM2->CNT > 60000U) return 0xFFFFU;
    }
    if (timeout == 0U) return 0xFFFFU;

    end_time = TIM2->CCR2;
    TIM2->SR &= ~TIM_SR_CC2IF;

    /* pulse width in us */
    pulse_width = (end_time >= start_time)
                ? (end_time - start_time)
                : (0xFFFFU - start_time + end_time + 1U);

    return (pulse_width / 58U);
}
