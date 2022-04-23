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

#include <stdbool.h>
#include "memory/mm.h"
#include "klibc/errno.h"
#include "debug/debug.h"
#include "defs.h"
#include "asm.h"


#define PEEK_ADDR	0xFFFF800000000000
#define M1GB		0x40000000
#define ENTRY_CNT	512

#define CR3_FLAG_MASK	(3ul << 3)
#define CR3_ADDR_MASK	0x000FFFFFFFFFF000
#define PML4_FLAG_MASK	0x1F
#define PDPE_FLAG_MASK	0x9F
#define PTE_FLAG_MASK	0x19F

#define PCD		(1ul << 4)
#define PWT		(1ul << 3)

#define FREE_RAM	1
#define RSRVD_RAM	2
#define ACPI_RAM	3
#define NVS_RAM		4
#define BAD_RAM		5


struct page_entry
{
	uint64_t flags	:	9;
	uint64_t avl2	:	3;
	uint64_t addr	:	40;
	uint64_t avl1	:	11;
	uint64_t nx	:	1;
};

__packed struct memmap
{
	uint64_t base;
	uint64_t len;
	uint32_t type;
	uint32_t attrs;
};


static struct page_entry *peek_pde;


static struct page_entry gen_entry(const void *addr, bool nx, uint16_t flags)
{
	struct page_entry tmp = {0};
	uint64_t ptr = PTR_TO_UINT(addr);

	tmp.addr = (ptr >> 12);
	tmp.flags = flags;
	if (nx) {
		tmp.nx = 1;
	}

	return tmp;
}

static struct page_entry gen_pml4(const void *addr, bool nx, uint16_t flags)
{
	flags &= PML4_FLAG_MASK;
	return gen_entry(addr, nx, flags);
}

static struct page_entry gen_pdpe_pde(const void *addr, bool nx, uint16_t flags)
{
	flags &= PDPE_FLAG_MASK;
	return gen_entry(addr, nx, flags);
}

static struct page_entry gen_pte(const void *addr, bool nx, uint16_t flags)
{
	flags &= PTE_FLAG_MASK;
	return gen_entry(addr, nx, flags);
}


void mm_init(void *peek_entry,
	     void *memmap,
	     size_t memmap_size,
	     void *load_buf,
	     void *load_end)
{
	size_t i;
	size_t valid_cnt;
	struct memmap *map;
	uint64_t max;

	peek_pde = peek_entry;
	map = memmap;
	max = 0;

	load_end = PAGE_UP_PTR(load_end);

	valid_cnt = 0;
	for (i = 0; i < memmap_size; i++) {
		map[i].len = PAGE_DOWN(map[i].base + map[i].len);
		map[i].base = PAGE_UP(map[i].base);
		map[i].len = map[i].len - map[i].base;

		if ((map[i].type == FREE_RAM) && (map[i].base >= 0x100000)) {
			valid_cnt++;
			debug_info("Base");
			debug_num(map[i].base);
			debug_info("Length");
			debug_num(map[i].len);
		}

		if ((map[i].base + map[i].len) > max) {
			max = (map[i].base + map[i].len);
		}
	}

	debug_info("Max address");
	debug_num(max);
}

int mm_peek(void **virt, void *phys, size_t size)
{
	void *tmp;
	uint64_t addr;
	size_t cnt;
	size_t i;

	if (peek_pde == NULL) {
		return EPERM;
	}

	if (virt == NULL) {
		return EINVAL;
	}

	if (size > M1GB) {
		return ENOTSUP;
	}

	addr = PTR_TO_UINT(phys);
	cnt = BLOCK_UP(addr + size) - BLOCK_DOWN(addr);
	cnt /= BLOCK_SIZE;

	if (cnt > ENTRY_CNT) {
		return ENOTSUP;
	}

	for (i = 0; i < cnt; i++) {
		tmp = UINT_TO_PTR(PAGE_DOWN(addr + (i * BLOCK_SIZE)));
		peek_pde[i] = gen_pdpe_pde(tmp, false, 0x83);
		invlpg(UINT_TO_PTR(PEEK_ADDR + (i * BLOCK_SIZE)));
	}

	tmp = UINT_TO_PTR(PEEK_ADDR + (addr % BLOCK_SIZE));
	*virt = tmp;

	return 0;
}

void mm_set_cr3(void *pml4, uint64_t flags)
{
	uint64_t temp;

	temp = PTR_TO_UINT(pml4) & CR3_ADDR_MASK;
	temp ^= (flags & CR3_FLAG_MASK);

	set_cr3(temp);
}
