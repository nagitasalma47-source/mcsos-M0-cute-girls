#include <mcsos/arch/cpu.h>

void kmain(void);

void _start(void) {
    kmain();

    cpu_halt_forever();
}
