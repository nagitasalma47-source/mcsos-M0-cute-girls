#include <stdint.h>
#include <mcsos/arch/idt.h>
#include <mcsos/kernel/log.h>
#include <pic.h>
#include <pit.h>
#include <mcsos/kernel/panic.h>
#include "vmm.h"

static const char *exception_names[32] = {
"#DE Divide Error",
"#DB Debug",
"NMI Interrupt",
"#BP Breakpoint",
"#OF Overflow",
"#BR Bound Range Exceeded",
"#UD Invalid Opcode",
"#NM Device Not Available",
"#DF Double Fault",
"Coprocessor Segment Overrun",
"#TS Invalid TSS",
"#NP Segment Not Present",
"#SS Stack Segment Fault",
"#GP General Protection Fault",
"#PF Page Fault",
"Reserved",
"#MF x87 Floating-Point Exception",
"#AC Alignment Check",
"#MC Machine Check",
"#XM SIMD Floating-Point Exception",
"#VE Virtualization Exception",
"#CP Control Protection Exception",
"Reserved",
"Reserved",
"Reserved",
"Reserved",
"Reserved",
"Reserved",
"#HV Hypervisor Injection Exception",
"#VC VMM Communication Exception",
"#SX Security Exception",
"Reserved"
};

static uint64_t trap_count;

static void page_fault_dump(uint64_t error_code, const x86_64_trap_frame_t *tf) {
    uint64_t cr2 = vmm_read_cr2();

    log_writeln("#PF page fault");

    log_key_value_hex64("cr2", cr2);
    log_key_value_hex64("error", error_code);
    log_key_value_hex64("rip", tf->rip);

    log_key_value_hex64("present_protection", (error_code & 1ULL) != 0ULL);
    log_key_value_hex64("write", (error_code & 2ULL) != 0ULL);
    log_key_value_hex64("user", (error_code & 4ULL) != 0ULL);
    log_key_value_hex64("reserved", (error_code & 8ULL) != 0ULL);
    log_key_value_hex64("instruction_fetch", (error_code & 16ULL) != 0ULL);
}


static const char *trap_name(uint64_t vector) {
if (vector < 32u) {
return exception_names[vector];
}

return "external-or-user-defined-interrupt";
}

uint64_t m4_trap_count_for_test(void) {
return trap_count;
}

static void log_trap_frame(const x86_64_trap_frame_t *frame) {
log_key_value_hex64("trap_vector", frame->vector);
log_key_value_hex64("trap_error", frame->error_code);
log_key_value_hex64("trap_rip", frame->rip);
log_key_value_hex64("trap_cs", frame->cs);
log_key_value_hex64("trap_rflags", frame->rflags);
log_key_value_hex64("trap_rax", frame->rax);
log_key_value_hex64("trap_rbx", frame->rbx);
log_key_value_hex64("trap_rcx", frame->rcx);
log_key_value_hex64("trap_rdx", frame->rdx);
}

void x86_64_trap_dispatch(x86_64_trap_frame_t *frame) {
KERNEL_ASSERT(frame != (x86_64_trap_frame_t *)0);

++trap_count;

log_write("[M4] trap dispatch: ");
log_writeln(trap_name(frame->vector));

log_trap_frame(frame);
if (frame->vector >= 32u && frame->vector <= 47u) {

    if (frame->vector == 32u) {
        timer_on_irq0();
    }

    pic_send_eoi((uint8_t)(frame->vector - 32u));
    return;
}
if (frame->vector == 14u) {
page_fault_dump(frame->error_code, frame);
KERNEL_PANIC("page fault", frame->error_code);
}

if (frame->vector == 3u) {
log_writeln("[M4] breakpoint handled; returning with iretq");
return;
}

KERNEL_PANIC("unrecoverable CPU exception", frame->vector);
}
