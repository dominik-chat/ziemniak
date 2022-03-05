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

#include <stdint.h>
#include "defs.h"
#include "interrupt.h"


#define PRES_F		(1<<7)
#define RING0_F		(0<<5)
#define RING1_F		(1<<5)
#define RING2_F		(2<<5)
#define RING3_F		(3<<5)
#define G_TASK_32	(0x05)
#define G_IRQ_16	(0x06)
#define G_TRAP_16	(0x07)
#define G_IRQ_32	(0x0E)
#define G_TRAP_32	(0x0F)


__packed struct int_frame
{
	uint32_t EIP;
	uint32_t CS;	/* 16 bit used */
	uint32_t FLAGS;
	uint32_t ESP;
	uint32_t SS;	/* 16 bit used */
};

__packed struct idt_entry
{
	uint16_t offset_low;
	uint16_t sel;
	uint8_t zero;
	uint8_t attr;
	uint16_t offset_high;
};


static struct idt_entry idt[48];
static volatile uint8_t *data_ready;


static inline void sti(void)
{
	asm volatile ("sti");
}

static inline void outb(uint16_t port, uint8_t val)
{
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline void lidt(void* base, uint16_t size)
{
	struct {
		uint16_t	length;
		void*		base;
	} __packed IDTR = {size, base};

	asm ("lidt %0" : : "m"(IDTR));
}

static void gen_entry(uint8_t id, uint16_t sel, void *offset, uint8_t attr)
{
	uint32_t off = (uint32_t)offset;

	idt[id].offset_low = (uint16_t)(off & 0xFFFF);
	idt[id].offset_high = (uint16_t)((off >> 16) & 0xFFFF);
	idt[id].zero = 0x00;
	idt[id].sel = sel;
	idt[id].attr = attr;
}

static void remap_pic(void)
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x00);
	outb(0xA1, 0x00);
}

__irq static void exception(struct int_frame *frame)
{
	UNUSED(frame);
	while(1){}
}

__irq static void exception_err(struct int_frame *frame, uint32_t err)
{
	UNUSED(frame);
	UNUSED(err);
	while(1){}
}

__irq static void irq_low(struct int_frame *frame)
{
	UNUSED(frame);
	outb(0x20, 0x20); //EOI
}

__irq static void irq_high(struct int_frame *frame)
{
	UNUSED(frame);
	outb(0xA0, 0x20);
	outb(0x20, 0x20); //EOI
}

__irq static void irq_disk1(struct int_frame *frame)
{
	UNUSED(frame);
	*data_ready |= 0x01;
	outb(0xA0, 0x20);
	outb(0x20, 0x20); //EOI
}

__irq static void irq_disk2(struct int_frame *frame)
{
	UNUSED(frame);
	*data_ready |= 0x02;
	outb(0xA0, 0x20);
	outb(0x20, 0x20); //EOI
}

void init_idt(volatile uint8_t *drdy)
{
	uint8_t entry;

	data_ready = drdy;

	gen_entry(0, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(1, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(2, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(3, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(4, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(5, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(6, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(7, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(8, 0x08, exception_err, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(9, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(10, 0x08, exception_err, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(11, 0x08, exception_err, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(12, 0x08, exception_err, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(13, 0x08, exception_err, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(14, 0x08, exception_err, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(15, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(16, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(17, 0x08, exception_err, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(18, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(19, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(20, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(21, 0x08, exception_err, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(22, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(23, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(24, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(25, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(26, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(27, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(28, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(29, 0x08, exception_err, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(30, 0x08, exception_err, (PRES_F | RING0_F |G_TRAP_32));
	gen_entry(31, 0x08, exception, (PRES_F | RING0_F |G_TRAP_32));

	for (entry = 32; entry < 40; entry++)
	{
		gen_entry(entry, 0x08, irq_low, (PRES_F | RING0_F |
								G_IRQ_32));
	}
	for (entry = 40; entry < 46; entry++)
	{
		gen_entry(entry, 0x08, irq_high, (PRES_F | RING0_F |
								G_IRQ_32));
	}

	gen_entry(46, 0x08, irq_disk1, (PRES_F | RING0_F | G_IRQ_32));
	gen_entry(47, 0x08, irq_disk2, (PRES_F | RING0_F | G_IRQ_32));

	lidt(idt, sizeof(idt) - 1);
	remap_pic();
	sti();
}
