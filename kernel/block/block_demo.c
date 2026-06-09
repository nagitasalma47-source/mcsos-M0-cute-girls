#include "mcsos/block.h"
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>

static unsigned char g_m14_ramdisk_storage[512u * 64u];
static mcsos_blk_device_t g_m14_ramdisk_dev;
static mcsos_ramblk_t g_m14_ramdisk;

void m14_block_demo_init(void) {
    mcsos_blk_registry_reset();
    mcsos_blk_status_t st = mcsos_ramblk_init(
        &g_m14_ramdisk_dev,
        &g_m14_ramdisk,
        "ram0",
        g_m14_ramdisk_storage,
        sizeof(g_m14_ramdisk_storage),
        512u);

    if (st != MCSOS_BLK_OK) {
        log_writeln("[M14] block layer init FAILED");
        return;
    }

    (void)mcsos_blk_register(&g_m14_ramdisk_dev);
    log_writeln("[M14] block layer initialized");
    log_writeln("[M14] ram0: 64 blocks x 512 bytes registered");
}
