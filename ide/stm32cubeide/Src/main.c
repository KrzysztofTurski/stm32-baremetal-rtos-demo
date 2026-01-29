#include <stdint.h>
#include "stm32f4xx.h"

extern void app_start(void);

int main(void)
{
    app_start();
    while (1) {}
}
