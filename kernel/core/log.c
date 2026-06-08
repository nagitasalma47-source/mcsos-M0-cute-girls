#include <stdint.h>
#include <mcsos/kernel/log.h>

void serial_init(void);
void serial_write(const char *s);
void serial_putc(char c);

static volatile int log_lock = 0;

static inline void lock(void)
{
    while (__atomic_test_and_set(&log_lock, __ATOMIC_ACQUIRE)) {}
}

static inline void unlock(void)
{
    __atomic_clear(&log_lock, __ATOMIC_RELEASE);
}

void log_init(void)
{
    serial_init();
}

void log_write(const char *s)
{
    if (!s) return;

    lock();
    serial_write(s);
    unlock();
}

void log_writeln(const char *s)
{
    if (!s) return;

    lock();
    serial_write(s);
    serial_write("\r\n");
    unlock();
}

void log_putc(char c)
{
    serial_putc(c);
}

void log_hex64(uint64_t value)
{
    static const char digits[] = "0123456789abcdef";

    log_write("0x");

    for (int shift = 60; shift >= 0; shift -= 4) {
        uint8_t nibble = (value >> shift) & 0xF;
        log_putc(digits[nibble]);
    }
}

void log_key_value_hex64(const char *key, uint64_t value)
{
    log_write(key);
    log_write("=");
    log_hex64(value);
    log_putc('\n');
}
