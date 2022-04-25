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

#include "memory/mm.h"
#include "klibc/errno.h"
#include "defs.h"


static size_t get_req(uint64_t max_addr)
{
	UNUSED(max_addr);
	return 0;
}

static int init(void *buf)
{
	UNUSED(buf);
	return 0;
}

static int alloc(void **buf, size_t len)
{
	UNUSED(buf);
	UNUSED(len);
	return 0;
}

static int free(void *buf, size_t len)
{
	UNUSED(buf);
	UNUSED(len);
	return 0;
}

struct allocator_api bitmap_allocator = {
	.get_req = get_req,
	.init = init,
	.alloc = alloc,
	.free = free
};
