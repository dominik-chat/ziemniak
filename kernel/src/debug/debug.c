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

void debug_num(const char *msg, uint64_t num)
{
	print_msg(msg, hex(num));
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

void debug_num(const char *msg, uint64_t num)
{
	UNUSED(num);
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
	debug_num("ERROR CODE: ", data->err);
	debug_num("RAX: ", data->rax);
	debug_num("RBX: ", data->rbx);
	debug_num("RCX: ", data->rcx);
	debug_num("RDX: ", data->rdx);
	debug_num("RSP: ", data->rsp);
	debug_num("RBP: ", data->rbp);
	debug_num("RSI: ", data->rsi);
	debug_num("RDI: ", data->rdi);
	debug_num("R8: ", data->r8);
	debug_num("R9: ", data->r9);
	debug_num("R10: ", data->r10);
	debug_num("R11: ", data->r11);
	debug_num("R12: ", data->r12);
	debug_num("R13: ", data->r13);
	debug_num("R14: ", data->r14);
	debug_num("R15: ", data->r15);
	debug_num("RIP: ", data->rip);
	debug_num("RFLAGS: ", data->rflags);
	debug_num("CR0: ", data->cr0);
	debug_num("CR2: ", data->cr2);
	debug_num("CR3: ", data->cr3);
	debug_num("CR4: ", data->cr4);
	debug_num("CS: ", data->cs);
	debug_num("SS: ", data->ss);
}
