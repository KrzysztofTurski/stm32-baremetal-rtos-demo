#include "lps25hb.h"
#include "i2c1.h"

/*
    PB8 ---- SCL
    PB9 ---- SDA
*/

//#define GPIOBEN                 (1U<<1)
//#define I2C1EN                  (1U<<21)

//#define I2C_100KHZ              80      // Standard mode 100kHz
//#define SD_MODE_MAX_RISE_TIME   17
#define LPS25HB_ADDR 0x5C
//#define I2C_TIMEOUT 100000



void LPS25HB_init(void)
{
    // CTRL_REG1 = 0x20
    // PD = 1, ODR = 12.5Hz, BDU = 1  → 0xB4

    I2C1_byteWrite(LPS25HB_ADDR, 0x20, 0xB4);
}


uint32_t LPS25HB_readPressure(void) {
    char data[3];
    I2C1_burstRead(LPS25HB_ADDR, 0x28 | 0x80, 3, data); // 0x80 = auto increment
    return ((uint32_t)data[2] << 16) | ((uint32_t)data[1] << 8) | data[0];
}



int16_t LPS25HB_readTemperature(void) {
    char data[2];
    I2C1_burstRead(LPS25HB_ADDR, 0x2B | 0x80, 2, data);
    return (int16_t)((data[1] << 8) | data[0]);
}

uint8_t LPS25HB_readWhoAmI(void)
{
    uint8_t who;
    I2C1_byteRead(LPS25HB_ADDR, 0x0F, &who);
    return who;
}
