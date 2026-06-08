#include <mcsos/arch/idt.h>
#include <mcsos/syscall.h>

#define IDT_GATE_TRAP 0x8Fu

extern void x86_64_syscall_int80_stub(void);

void syscall_arch_init(void) {
    x86_64_idt_set_gate(
        0x80,
        (uint64_t)(uintptr_t)x86_64_syscall_int80_stub,
        IDT_GATE_TRAP
    );
}
