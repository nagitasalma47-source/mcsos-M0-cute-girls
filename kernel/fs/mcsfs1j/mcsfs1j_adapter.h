#pragma once

#include <stdint.h>

int m16_format(void *block_device_context);
int m16_mount(void *block_device_context, void *out_super);
int m16_fsck(void *block_device_context);

int m16_write_file(
    void *block_device_context,
    const char *name,
    const uint8_t *data,
    uint32_t size
);

int m16_read_file(
    void *block_device_context,
    const char *name,
    uint8_t *out,
    uint32_t cap,
    uint32_t *out_size
);
