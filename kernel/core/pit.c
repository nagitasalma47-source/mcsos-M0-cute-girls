#include <pit.h>
#include <io.h>
#include <serial.h>

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40
#define PIT_BASE_FREQUENCY 1193182

volatile uint64_t g_ticks = 0;

void pit_configure_hz(uint32_t hz) {
    uint16_t divisor = (uint16_t)(PIT_BASE_FREQUENCY / hz);

    outb(PIT_COMMAND, 0x36);

    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

void timer_on_irq0(void) {
    g_ticks++;

    if ((g_ticks % 100) == 0) {
        serial_write("[MCSOS:TIMER] tick\n");
    }

    outb(0x20, 0x20);
}
