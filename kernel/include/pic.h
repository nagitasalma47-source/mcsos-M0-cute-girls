#ifndef MCSOS_PIC_H
#define MCSOS_PIC_H

#include <stdint.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC_EOI      0x20

#define PIC1_OFFSET  0x20
#define PIC2_OFFSET  0x28

void pic_remap(int offset1, int offset2);
void pic_mask_all(void);
void pic_unmask_irq(uint8_t irq);
void pic_send_eoi(uint8_t irq);

#endif