#include <limine.h>
__attribute__((used, section(".limine_requests")))
volatile LIMINE_BASE_REVISION(0);

#include <stdint.h>

#include <mcsos/arch/cpu.h>
#include <mcsos/arch/idt.h>
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>
#include <io.h>
#include <pic.h>
#include <pit.h>
#include <mcsos/kernel/version.h>
#include "pmm.h"
#include "vmm.h"
#include "mcsos/kmem.h"

extern char __kernel_start[];
extern char __kernel_end[];

static struct pmm_state kernel_pmm;
static uint8_t kernel_pmm_bitmap[PMM_BITMAP_BYTES] __attribute__((aligned(4096)));

static struct vmm_space kernel_space;
static uint64_t hhdm_offset = 0;

#define M8_BOOT_HEAP_SIZE (64u * 1024u)
#define KHEAP_BASE 0xffffffff90000000ull
#define KHEAP_SIZE (256ull * 1024ull)
static unsigned char m8_boot_heap[M8_BOOT_HEAP_SIZE] __attribute__((aligned(4096)));

static void m8_heap_bootstrap(void) {
    int rc = kmem_init(m8_boot_heap, sizeof(m8_boot_heap));

    if (rc != 0) {
        kernel_panic("M8 kmem_init failed");
    }

    void *probe = kmem_alloc(128);

    if (probe == 0) {
        kernel_panic("M8 kmem_alloc probe failed");
    }

    if (kmem_free_checked(probe) != 0) {
        kernel_panic("M8 kmem_free_checked probe failed");
    }

    kmem_stats_t st;
    kmem_get_stats(&st);

    klog_info("M8 kmem initialized");
}
static int kheap_map_initial_pages(void) {
    for (uint64_t va = KHEAP_BASE; 
         va < KHEAP_BASE + KHEAP_SIZE; 
         va += 4096ull) {

        uint64_t pa = pmm_alloc_frame(&kernel_pmm);

        if (pa == PMM_INVALID_FRAME) {
            return -1;
        }

        int rc = vmm_map_page(
            &kernel_space,
            va,
            pa,
            VMM_PRESENT | VMM_WRITABLE
        );

        if (rc != VMM_MAP_OK) {
            pmm_free_frame(&kernel_pmm, pa);
            return -2;
        }
    }

    return kmem_init((void *)KHEAP_BASE, KHEAP_SIZE);
}

static void memzero(void *ptr, uint64_t size) {
    uint8_t *p = (uint8_t *)ptr;
    for (uint64_t i = 0; i < size; i++) {
        p[i] = 0;
    }
}

static uint64_t kernel_vmm_alloc(void *ctx) {
    (void)ctx;
    return pmm_alloc_frame(&kernel_pmm);
}

static void kernel_vmm_free(void *ctx, uint64_t frame_paddr) {
    (void)ctx;
    pmm_free_frame(&kernel_pmm, frame_paddr);
}

static void *kernel_phys_to_virt(void *ctx, uint64_t paddr) {
    uint64_t offset = *(uint64_t *)ctx;
    return (void *)(uintptr_t)(offset + paddr);
}

static struct boot_mem_region test_regions[] = {
    { .base = 0x00000000ULL, .length = 0x0009f000ULL, .type = BOOT_MEM_USABLE },
    { .base = 0x0009f000ULL, .length = 0x00001000ULL, .type = BOOT_MEM_RESERVED },
    { .base = 0x00100000ULL, .length = 0x03f00000ULL, .type = BOOT_MEM_USABLE },
};

static void m4_selftest(void) {
KERNEL_ASSERT(__kernel_end > __kernel_start);
KERNEL_ASSERT(sizeof(uintptr_t) == 8u);
KERNEL_ASSERT(sizeof(x86_64_idt_entry_t) == 16u);
KERNEL_ASSERT(x86_64_idt_base_for_test() != 0u);
KERNEL_ASSERT(x86_64_idt_limit_for_test() == 4095u);

log_writeln("[M4] selftest: IDT invariants passed");
}

void kmain(void) {
log_init();

log_write(MCSOS_NAME);
log_write(" ");
log_write(MCSOS_VERSION);
log_write(" ");
log_write(MCSOS_MILESTONE);
log_writeln(" kernel entered");

log_key_value_hex64(
"kernel_start",
(uint64_t)(uintptr_t)__kernel_start
);

log_key_value_hex64(
"kernel_end",
(uint64_t)(uintptr_t)__kernel_end
);

log_key_value_hex64(
"rflags_before_idt",
cpu_read_rflags()
);

x86_64_idt_init();
cpu_cli();

pic_remap(PIC1_OFFSET, PIC2_OFFSET);

pic_mask_all();
pic_unmask_irq(0);
pit_configure_hz(100);
cpu_sti();


log_writeln("[M5] PIC/PIT initialized; interrupts enabled");

m4_selftest();

if (!pmm_init_from_map(
    &kernel_pmm,
    test_regions,
    sizeof(test_regions) / sizeof(test_regions[0]),
    kernel_pmm_bitmap,
    sizeof(kernel_pmm_bitmap),



    64ULL * 1024ULL * 1024ULL
)) {
    KERNEL_PANIC("pmm_init_from_map failed", 0x4D3650414E4943ULL);
}

log_writeln("[M6] PMM initialized");

uint64_t frame = pmm_alloc_frame(&kernel_pmm);

if (frame == PMM_INVALID_FRAME) {
    KERNEL_PANIC("pmm_alloc_frame failed", 0x4D3650414E4943ULL);
}

log_key_value_hex64("[M6] allocated_frame", frame);

if (!pmm_free_frame(&kernel_pmm, frame)) {
    KERNEL_PANIC("pmm_free_frame failed", 0x4D3650414E4943ULL);
}

log_writeln("[M6] PMM selftest passed");

uint64_t root = pmm_alloc_frame(&kernel_pmm);

if (root == PMM_INVALID_FRAME) {
    KERNEL_PANIC("M7: cannot allocate root page table", 0x4D3750414E4943ULL);
}

void *root_virt = kernel_phys_to_virt(&hhdm_offset, root);

memzero(root_virt, 4096);

int vmm_rc = vmm_space_init(
    &kernel_space,
    root,
    &hhdm_offset,
    kernel_vmm_alloc,
    kernel_vmm_free,
    kernel_phys_to_virt
);

if (vmm_rc != VMM_MAP_OK) {
    KERNEL_PANIC("M7: vmm_space_init failed", 0x4D3750414E4943ULL);
}

log_writeln("[M7] VMM core initialized");

m8_heap_bootstrap();
log_writeln("[M8] kernel heap initialized");


#ifdef MCSOS_M4_TRIGGER_BREAKPOINT
log_writeln("[M4] triggering intentional breakpoint exception");
x86_64_trigger_breakpoint_for_test();
log_writeln("[M4] returned from breakpoint handler");
#endif

#ifdef MCSOS_M4_TRIGGER_PANIC
KERNEL_PANIC(
"intentional M4 panic test",
0x4D43534F533034u
);
#else
log_writeln("[M4] IDT and exception dispatch path installed");
log_writeln("[M4] ready for QEMU smoke test and GDB audit");

cpu_halt_forever();
#endif
}
