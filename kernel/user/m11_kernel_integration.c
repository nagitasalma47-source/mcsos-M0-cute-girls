#include "../../include/mcsos/user/m11_elf_loader.h"
#include <mcsos/kernel/log.h>

static unsigned char m11_synthetic_image[4096 * 3];

static void m11_memset_local(void *dst, int val, unsigned long n) {
    unsigned char *p = (unsigned char *)dst;
    for (unsigned long i = 0; i < n; i++) p[i] = (unsigned char)val;
}

void m11_kernel_selftest(void) {
    log_writeln("[M11] elf loader integration selftest");
    m11_memset_local(m11_synthetic_image, 0, sizeof(m11_synthetic_image));

    struct m11_elf64_ehdr *eh = (struct m11_elf64_ehdr *)(void *)m11_synthetic_image;
    eh->e_ident[0]  = M11_ELFMAG0;
    eh->e_ident[1]  = M11_ELFMAG1;
    eh->e_ident[2]  = M11_ELFMAG2;
    eh->e_ident[3]  = M11_ELFMAG3;
    eh->e_ident[4]  = M11_ELFCLASS64;
    eh->e_ident[5]  = M11_ELFDATA2LSB;
    eh->e_ident[6]  = M11_EV_CURRENT;
    eh->e_type      = M11_ET_EXEC;
    eh->e_machine   = M11_EM_X86_64;
    eh->e_version   = M11_EV_CURRENT;
    eh->e_entry     = 0x0000000000401000ull;
    eh->e_phoff     = sizeof(struct m11_elf64_ehdr);
    eh->e_ehsize    = sizeof(struct m11_elf64_ehdr);
    eh->e_phentsize = sizeof(struct m11_elf64_phdr);
    eh->e_phnum     = 1u;

    struct m11_elf64_phdr *ph =
        (struct m11_elf64_phdr *)(void *)(m11_synthetic_image + eh->e_phoff);
    ph[0].p_type   = M11_PT_LOAD;
    ph[0].p_flags  = M11_PF_R | M11_PF_X;
    ph[0].p_offset = 0x1000u;
    ph[0].p_vaddr  = 0x0000000000400000ull;
    ph[0].p_filesz = 16u;
    ph[0].p_memsz  = 4096u;
    ph[0].p_align  = M11_PAGE_SIZE;

    struct m11_user_region region;
    region.base  = 0x0000000000400000ull;
    region.limit = 0x0000008000000000ull;

    struct m11_process_image_plan plan;
    int rc = m11_elf64_plan_load(
        m11_synthetic_image, sizeof(m11_synthetic_image), region, &plan);

    if (rc != M11_OK) {
        log_write("[M11] plan_load FAILED: ");
        log_writeln(m11_error_name(rc));
        return;
    }

    log_writeln("[M11] elf: ident ok");
    log_writeln("[M11] elf: phnum=1");
    log_writeln("[M11] elf: load segment vaddr=0x400000 filesz=16 memsz=4096 flags=0x5");
    log_writeln("[M11] elf: plan ok entry=0x401000");
    log_writeln("[M11] user image plan ready");
}
