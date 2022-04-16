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

#include "serial/serial.h"
#include "debug/debug.h"
#include "defs.h"


int debug_init(void)
{
	return serial_init(0, NULL);
}

#ifdef DEBUG
static void print_msg(const char *header, const char *msg)
{
	serial_print(0, header);
	serial_print(0, msg);
	serial_print(0, "\n");
}

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

void debug_crash(int excep)
{
	UNUSED(excep);
}
