#include <stdint.h>
#include <mcsos/arch/io.h>

#define COM1_PORT 0x3F8u

static volatile int serial_lock = 0;

/* ================= LOW LEVEL ================= */

static int serial_tx_ready(void)
{
    return (inb(COM1_PORT + 5u) & 0x20u) != 0;
}

void serial_init(void)
{
    outb(COM1_PORT + 1u, 0x00u);
    outb(COM1_PORT + 3u, 0x80u);
    outb(COM1_PORT + 0u, 0x03u);
    outb(COM1_PORT + 1u, 0x00u);
    outb(COM1_PORT + 3u, 0x03u);
    outb(COM1_PORT + 2u, 0xC7u);
    outb(COM1_PORT + 4u, 0x0Bu);
}

void serial_putc(char c)
{
    uint32_t timeout = 1000000u;

    while (!serial_tx_ready()) {
        if (timeout-- == 0u) {
            return;
        }
    }

    outb(COM1_PORT, (uint8_t)c);
}

/* ================= FINAL SAFE WRITE ================= */

void serial_write(const char *s)
{
    if (!s) return;

    while (__atomic_test_and_set(&serial_lock, __ATOMIC_ACQUIRE)) {
        // spin
    }

    while (*s) {
        if (*s == '\n') {
            serial_putc('\r');
        }
        serial_putc(*s++);
    }

    __atomic_clear(&serial_lock, __ATOMIC_RELEASE);
}