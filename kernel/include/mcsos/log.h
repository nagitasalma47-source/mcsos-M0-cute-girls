#ifndef MCSOS_LOG_H
#define MCSOS_LOG_H

#include <stdarg.h>

void log_init(void);
void log_putc(char c);
void log_write(const char *s);
void log_printf(const char *fmt, ...);
void log_vprintf(const char *fmt, va_list ap);

#endif
