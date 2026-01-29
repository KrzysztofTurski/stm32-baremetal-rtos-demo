#ifndef LOG_H_
#define LOG_H_

/* Thread-safe logging (serializes access to USART) */
void log_init(void);
void log_write(const char *s);

#endif
