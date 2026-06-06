#include "mcs_sync.h"

extern void klog_info(const char *msg);
extern void kernel_panic(const char *msg);

static mcs_spinlock_t boot_stats_lock;
static mcs_lockdep_state_t boot_lockdep;
static uint64_t boot_counter;

void m12_sync_selftest(void) {
    mcs_lockdep_init(&boot_lockdep);
    mcs_spin_init(&boot_stats_lock, 10u, "boot_stats");

    if (mcs_lockdep_before_acquire(&boot_lockdep, 10u, "boot_stats") != MCS_SYNC_OK) {
        kernel_panic("M12 lockdep acquire failed");
    }

    mcs_spin_lock(&boot_stats_lock);
    boot_counter++;
    mcs_spin_unlock(&boot_stats_lock);

    if (mcs_lockdep_after_release(&boot_lockdep, 10u, "boot_stats") != MCS_SYNC_OK) {
        kernel_panic("M12 lockdep release failed");
    }

    klog_info("M12 sync selftest passed");
}
