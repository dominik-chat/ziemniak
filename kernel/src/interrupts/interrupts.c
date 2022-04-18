/*
 * Copyright (C) 2022 Dominik Chat
 *
 * This file is part of Ziemniak.
 *
 * Ziemniak is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Ziemniak is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ziemniak. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stddef.h>
#include <stdint.h>
#include "interrupts/interrupts.h"
#include "interrupts/wrapper_exc.h"
#include "interrupts/wrapper_pic.h"
#include "debug/debug.h"
#include "defs.h"
#include "asm.h"


#define PRESENT		0x80
#define GATE_INT	0x0E
#define GATE_TRAP	0x0F


__packed struct idt_entry
{
	uint16_t    isr_l;
	uint16_t    cs;
	uint8_t	    ist;
	uint8_t     attr;
	uint16_t    isr_m;
	uint32_t    isr_h;
	uint32_t    rsrvd;
};


static const void *isr_tab[48] =
{
	exc_wrapper_0,	exc_wrapper_1,	exc_wrapper_2,	exc_wrapper_3,
	exc_wrapper_4,	exc_wrapper_5,	exc_wrapper_6,	exc_wrapper_7,
	exc_wrapper_8,	exc_wrapper_9,	exc_wrapper_10,	exc_wrapper_11,
	exc_wrapper_12,	exc_wrapper_13,	exc_wrapper_14,	exc_wrapper_15,
	exc_wrapper_16,	exc_wrapper_17,	exc_wrapper_18,	exc_wrapper_19,
	exc_wrapper_20,	exc_wrapper_21,	exc_wrapper_22,	exc_wrapper_23,
	exc_wrapper_24,	exc_wrapper_25,	exc_wrapper_26,	exc_wrapper_27,
	exc_wrapper_28,	exc_wrapper_29,	exc_wrapper_30,	exc_wrapper_31,
	isr_wrapper_pic_0,	isr_wrapper_pic_1,	isr_wrapper_pic_2,
	isr_wrapper_pic_3,	isr_wrapper_pic_4,	isr_wrapper_pic_5,
	isr_wrapper_pic_6,	isr_wrapper_pic_7,	isr_wrapper_pic_8,
	isr_wrapper_pic_9,	isr_wrapper_pic_10,	isr_wrapper_pic_11,
	isr_wrapper_pic_12,	isr_wrapper_pic_13,	isr_wrapper_pic_14,
	isr_wrapper_pic_15
};


__align(16) static struct idt_entry idt[48];


static void gen_entry(struct idt_entry *entry,
		      const void *isr,
		      const uint8_t attr)
{
	uint64_t addr;

	addr = PTR_TO_UINT(isr);
	entry->isr_l = (addr & 0xFFFF);
	entry->cs = 0x08;
	entry->ist = 0x00;
	entry->attr = attr;
	entry->isr_m = ((addr >> 16) & 0xFFFF);
	entry->isr_h = ((addr >> 32) & 0xFFFFFFFF);
	entry->rsrvd = 0x00;
}

int interrupts_init(void)
{
	size_t i;

	for (i = 0; i < 32; i++) {
		gen_entry(&idt[i], isr_tab[i], (PRESENT | GATE_TRAP));
	}

	for (i = 32; i < 48; i++) {
		gen_entry(&idt[i], isr_tab[i], (PRESENT | GATE_INT));
	}

	lidt(idt, (sizeof(idt) - 1));

	/* Remap PIC to 32-47 */
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 40);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	sti();
	return 0;
}

void exception_handler(int number)
{
	debug_crash(number);

	while(1) {
	}
}

void isr_0(void)
{

}

void isr_1(void)
{

}

void isr_2(void)
{

}

void isr_3(void)
{

}

void isr_4(void)
{

}

void isr_5(void)
{

}

void isr_6(void)
{

}

void isr_7(void)
{

}

void isr_8(void)
{

}

void isr_9(void)
{

}

void isr_10(void)
{

}

void isr_11(void)
{

}

void isr_12(void)
{

}

void isr_13(void)
{

}

void isr_14(void)
{

}

void isr_15(void)
{

}
