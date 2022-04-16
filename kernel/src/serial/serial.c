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
#include <stdbool.h>
#include "serial/serial.h"
#include "klibc/errno.h"
#include "asm.h"
#include "defs.h"


#define SERIAL_CNT	4
#define DATA_REG	0
#define INTEN_REG	1
#define DIV_LOW_REG	0
#define DIV_HIGH_REG	1
#define INT_FIFO_REG	2
#define LINE_CTL_REG	3
#define MODEM_CTL_REG	4
#define LINE_STS_REG	5
#define MODEM_STS_REG	6
#define DLAB		0x80
#define TX_EMPTY	0x20


struct serial_ctx {
	serial_rx_t rx_cb;
	uint16_t port;
};


static const uint16_t *bda_coms = (const uint16_t *)0x0400;
static struct serial_ctx ctx[SERIAL_CNT];


static bool is_tx_empty(uint16_t port)
{
	return ((inb(port + LINE_STS_REG) & TX_EMPTY) != 0);
}

static void serial_putc(uint16_t port, char chr)
{
	while (!is_tx_empty(port));

	outb(port + DATA_REG, chr);
}


int serial_init(size_t id, serial_rx_t rx_cb)
{
	uint16_t port;

	if (id >= SERIAL_CNT) {
		return EINVAL;
	}

	port = bda_coms[id];

	if (port == 0) {
		return EIO;
	}

	/* Disable interrupts */
	outb(port + INTEN_REG, 0x00);
	/* Max speed - 115200 */
	outb(port + LINE_CTL_REG, DLAB);
	outb(port + DIV_LOW_REG, 0x01);
	outb(port + DIV_HIGH_REG, 0x00);
	/* Classic 8N+1S */
	outb(port + LINE_CTL_REG, 0x03);
	outb(port + INT_FIFO_REG, 0xC7);
	outb(port + MODEM_CTL_REG, 0x0B);

	/* Test */
	outb(port + MODEM_CTL_REG, 0x1E);
	outb(port + DATA_REG, 0x5A);
	if(inb(port + DATA_REG) != 0x5A) {
		return EIO;
	}

	ctx[id].rx_cb = rx_cb;
	ctx[id].port = port;

	outb(port + MODEM_CTL_REG, 0x0B);
	return 0;
}

int serial_print(size_t id, const char *text)
{
	size_t i;

	if (id >= SERIAL_CNT) {
		return EINVAL;
	}

	if (ctx[id].port == 0) {
		return EPERM;
	}

	for (i = 0; text[i] != '\0'; i++) {
		serial_putc(ctx[id].port, text[i]);
	}

	return 0;
}
