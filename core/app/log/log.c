#include "log.h"
#include "usart2.h"
#include "osMutex.h"

static osMutex_t logMutex;

void log_init(void)
{
    osMutexInit(&logMutex);
}

void log_write(const char *s)
{
    osMutexLock(&logMutex);
    usart2_write(s);
    osMutexUnlock(&logMutex);
}
