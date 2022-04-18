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
#include "serial/serial.h"
#include "debug/debug.h"
#include "defs.h"


struct dump_data {
	uint64_t cr4;
	uint64_t cr3;
	uint64_t cr2;
	uint64_t cr0;
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rbp;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	uint64_t err;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
};


static const char *error_msg[32] = {
	"DIVIDE BY ZERO",
	"DEBUG",
	"NON MASKABLE INTERRUPT",
	"BREAKPOINT",
	"OVERFLOW",
	"BOUND RANGE",
	"INVALID OPCODE",
	"FPU NOT PRESENT",
	"DOUBLE FAULT",
	"COPROCESSOR SEGMENT OVERRUN",
	"INVALID TASK STATE SEGMENT",
	"SEGMENT NOT PRESENT",
	"STACK SEGMENT FAULT",
	"GENERAL PROTECTION FAULT",
	"PAGE FAULT",
	"RESERVED",
	"X87 FLOATING POINT",
	"ALIGNMENT CHECK",
	"MACHINE CHECK",
	"SIMD",
	"VIRTUALIZATION",
	"CONTROL PROTECTION",
	"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
	"HYPERVISON INJECTION",
	"VMM COMMUNICATION",
	"SECURITY",
	"RESERVED"
};


static char *hex(uint64_t num)
{
	static char buf[19];
	uint8_t tmp;
	size_t i;

	buf[0] = '0';
	buf[1] = 'x';
	buf[18] = '\0';

	for (i = 0; i < 16; i++) {
		tmp = ((num >> (4 * (15 - i))) & 0x0F);

		if (tmp > 9) {
			buf[i + 2] = tmp - 10 + 'A';
		} else {
			buf[i + 2] = tmp + '0';
		}
	}

	return buf;
};

static void print_msg(const char *header, const char *msg)
{
	serial_print(0, header);
	serial_print(0, msg);
	serial_print(0, "\n");
}


int debug_init(void)
{
	return serial_init(0, NULL);
}

#ifdef DEBUG
void debug_info(const char *msg)
{
	print_msg("INFO: ", msg);
}

void debug_warn(const char *msg)
{
	print_msg("WARN: ", msg);
}

void debug_err(const char *msg)
{
	print_msg("ERR: ", msg);
}
#else /* DEBUG */
void debug_info(const char *msg)
{
	UNUSED(msg);
}

void debug_warn(const char *msg)
{
	UNUSED(msg);
}

void debug_err(const char *msg)
{
	UNUSED(msg);
}
#endif /* DEBUG */

void debug_crash(uint64_t excep, void *dump)
{
	struct dump_data *data;

	data = dump;

	print_msg("CRASH", "\0");
	if (excep < 32) {
		print_msg(error_msg[excep], " EXCEPTION");
	}
	print_msg("ERROR CODE: ", hex(data->err));
	print_msg("RAX: ", hex(data->rax));
	print_msg("RBX: ", hex(data->rbx));
	print_msg("RCX: ", hex(data->rcx));
	print_msg("RDX: ", hex(data->rdx));
	print_msg("RSP: ", hex(data->rsp));
	print_msg("RBP: ", hex(data->rbp));
	print_msg("RSI: ", hex(data->rsi));
	print_msg("RDI: ", hex(data->rdi));
	print_msg("R8: ", hex(data->r8));
	print_msg("R9: ", hex(data->r9));
	print_msg("R10: ", hex(data->r10));
	print_msg("R11: ", hex(data->r11));
	print_msg("R12: ", hex(data->r12));
	print_msg("R13: ", hex(data->r13));
	print_msg("R14: ", hex(data->r14));
	print_msg("R15: ", hex(data->r15));
	print_msg("RIP: ", hex(data->rip));
	print_msg("RFLAGS: ", hex(data->rflags));
	print_msg("CR0: ", hex(data->cr0));
	print_msg("CR2: ", hex(data->cr2));
	print_msg("CR3: ", hex(data->cr3));
	print_msg("CR4: ", hex(data->cr4));
	print_msg("CS: ", hex(data->cs));
	print_msg("SS: ", hex(data->ss));
}
