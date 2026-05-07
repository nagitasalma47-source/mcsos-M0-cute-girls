#ifndef MCSOS_PANIC_H
#define MCSOS_PANIC_H

#include <stdarg.h>

__attribute__((noreturn))
void panic(const char *fmt, ...);

__attribute__((noreturn))
void panic_v(const char *fmt, va_list ap);

#endif
