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

#ifndef _ASM_H_
#define _ASM_H_

#include <stdint.h>
#include "defs.h"


static inline void outb(uint16_t port, uint8_t val)
{
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

static inline void outsw(uint16_t port, const void *addr, uint64_t cnt)
{
	asm volatile("rep; outsw" : "+S"(addr), "+c"(cnt) : "d"(port));
}

static inline void insw(uint16_t port, void *addr, uint64_t cnt)
{
	asm volatile("rep; insw" : "+D"(addr), "+c"(cnt) : "d"(port));
}

static inline void cli(void)
{
	asm volatile ("cli");
}

static inline void sti(void)
{
	asm volatile ("sti");
}

static inline void lidt(void *idt, uint16_t len)
{
	struct {
		uint16_t len;
		void *idt;
	} __packed idtr = {len, idt};

	asm ("lidt %0" : : "m"(idtr));
}

#endif /* _ASM_H_ */
