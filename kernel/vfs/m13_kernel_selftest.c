#include "mcs_vfs.h"
#include <mcsos/kernel/log.h>

static mcs_ramfs_t kernel_ramfs;
static mcs_process_t kernel_proc_m13;

void m13_vfs_kernel_selftest(void) {
    int fd;
    char buf[16];
    mcs_ssize_t n;

    log_writeln("[M13] VFS/RAMFS kernel selftest begin");

    mcs_ramfs_init(&kernel_ramfs);
    log_writeln("[M13] ramfs init: OK");

    {
        static const uint8_t hello[] = "hello-mcsos";
        int rc = mcs_ramfs_seed_file(&kernel_ramfs, "/hello.txt",
                                     hello, sizeof(hello) - 1u);
        if (rc != MCS_OK) {
            log_writeln("[M13] seed_file FAILED");
            return;
        }
    }
    log_writeln("[M13] seed_file /hello.txt: OK");

    kernel_proc_m13.pid = 0xDu;
    mcs_fd_table_init(&kernel_proc_m13.fd_table);

    fd = mcs_sys_open(&kernel_proc_m13, &kernel_ramfs,
                      "/hello.txt", MCS_O_RDONLY);
    if (fd < 0) {
        log_writeln("[M13] sys_open FAILED");
        return;
    }
    log_writeln("[M13] sys_open /hello.txt: OK");

    /* read 5 bytes */
    for (int i = 0; i < 16; i++) buf[i] = 0;
    n = mcs_sys_read(&kernel_proc_m13, fd, buf, 5);
    if (n != 5) {
        log_writeln("[M13] sys_read FAILED");
        return;
    }
    log_writeln("[M13] sys_read 5 bytes: OK");

    if (mcs_sys_close(&kernel_proc_m13, fd) != MCS_OK) {
        log_writeln("[M13] sys_close FAILED");
        return;
    }
    log_writeln("[M13] sys_close: OK");

    /* test EBADF after close */
    n = mcs_sys_read(&kernel_proc_m13, fd, buf, 1);
    if (n != MCS_EBADF) {
        log_writeln("[M13] EBADF check FAILED");
        return;
    }
    log_writeln("[M13] EBADF after close: OK");

    /* test ENOENT */
    fd = mcs_sys_open(&kernel_proc_m13, &kernel_ramfs,
                      "/missing.txt", MCS_O_RDONLY);
    if (fd != MCS_ENOENT) {
        log_writeln("[M13] ENOENT check FAILED");
        return;
    }
    log_writeln("[M13] ENOENT missing file: OK");

    /* test create + write */
    fd = mcs_sys_open(&kernel_proc_m13, &kernel_ramfs,
                      "/log.txt",
                      MCS_O_CREAT | MCS_O_RDWR | MCS_O_TRUNC);
    if (fd < 0) {
        log_writeln("[M13] create log.txt FAILED");
        return;
    }
    n = mcs_sys_write(&kernel_proc_m13, fd, "OK13", 4);
    if (n != 4) {
        log_writeln("[M13] write log.txt FAILED");
        return;
    }
    if (mcs_sys_lseek(&kernel_proc_m13, fd, 0, MCS_SEEK_SET) != 0) {
        log_writeln("[M13] lseek log.txt FAILED");
        return;
    }
    for (int i = 0; i < 16; i++) buf[i] = 0;
    n = mcs_sys_read(&kernel_proc_m13, fd, buf, 4);
    if (n != 4) {
        log_writeln("[M13] read back log.txt FAILED");
        return;
    }
    mcs_sys_close(&kernel_proc_m13, fd);
    log_writeln("[M13] create/write/lseek/read /log.txt: OK");

    log_writeln("[M13] VFS/RAMFS kernel selftest: PASS");
}
