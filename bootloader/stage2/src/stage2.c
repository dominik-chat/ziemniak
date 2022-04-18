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
#include "video.h"
#include "cdrom.h"


#define FREE_RAM	0x01

#define PML4E	0
#define PDPE_T	1
#define PDPE_M	2
#define PDPE_B	3
#define PDE_KER	4
#define PDE_STA	5
#define PDE_MID	6
#define PDE_LOW	7
#define PTE_KER	8
#define PTE_STA	9


__packed struct real_mode_data
{
	uint32_t	framebuf_ptr;
	uint32_t	font_ptr;
	uint8_t		red_pos;
	uint8_t		green_pos;
	uint8_t		blue_pos;
	uint8_t		rsrvd_pos;
	uint16_t	memmap_seg;
	uint16_t	memmap_size;
	uint16_t	scr_width;
	uint16_t	scr_height;
	uint8_t		boot_disk;
	uint8_t		is_boot_disk_slv;
	uint16_t	boot_disk_io;
	uint16_t	boot_disk_ctl;
};

__packed struct long_mode_data
{
	uint64_t	framebuf_ptr;
	uint64_t	font_ptr;
	uint64_t	memmap_ptr;
	uint64_t	load_buf;
	uint64_t	peek_entry;
	uint16_t	memmap_size;
	uint16_t	scr_width;
	uint16_t	scr_height;
	uint16_t	boot_disk_io;
	uint16_t	boot_disk_ctl;
	uint8_t		is_boot_disk_slv;
	uint8_t		red_pos;
	uint8_t		green_pos;
	uint8_t		blue_pos;
	uint8_t		rsrvd_pos;
};

__packed struct mem_map
{
	uint64_t base;
	uint64_t len;
	uint32_t type;
	uint32_t attrs;
};

struct kernel_header
{
	uint64_t start;
	uint64_t stext;
	uint64_t etext;
	uint64_t sdata;
	uint64_t edata;
	uint64_t srodata;
	uint64_t erodata;
	uint64_t sbss;
	uint64_t ebss;
	uint64_t stack_size;
};

struct page
{
	uint64_t pres	:	1;
	uint64_t writ	:	1;
	uint64_t user	:	1;
	uint64_t wr_th	:	1;
	uint64_t d_cach	:	1;
	uint64_t accd	:	1;
	uint64_t dirty	:	1;
	uint64_t big	:	1;
	uint64_t glob	:	1;
	uint64_t avl2	:	3;
	uint64_t addr	:	40;
	uint64_t avl1	:	11;
	uint64_t nx	:	1;
};

extern struct real_mode_data rm_data;
extern void run(uint64_t addr, void *pml4e, void *lm_data);


static const char kernel_name[] = "POTATO.BIN";
static volatile uint8_t data_ready;
static uint16_t cdrom_buf[CD_SECTOR_SIZE/sizeof(uint16_t)];
static struct long_mode_data lm_data;


static void print_memmap(struct mem_map *mem_map)
{
	uint16_t i;

	video_print("Memory map:\n");

	for (i = 0; i < rm_data.memmap_size; i++) {
		video_print_hex(mem_map[i].base, 0);
		video_print_hex(mem_map[i].len, 0);
		video_print_hex(mem_map[i].type, 8);
		video_print_hex(mem_map[i].attrs, 8);
		video_print("\n");
	}
}

static void print_header(struct kernel_header *header)
{
	video_print("Header contents:\n");

	video_print("start:      ");
	video_print_hex(header->start, 0);
	video_print("\nstext:      ");
	video_print_hex(header->stext, 0);
	video_print("\netext:      ");
	video_print_hex(header->etext, 0);
	video_print("\nsdata:      ");
	video_print_hex(header->sdata, 0);
	video_print("\nedata:      ");
	video_print_hex(header->edata, 0);
	video_print("\nsrodata:    ");
	video_print_hex(header->srodata, 0);
	video_print("\nerodata:    ");
	video_print_hex(header->erodata, 0);
	video_print("\nsbss:       ");
	video_print_hex(header->sbss, 0);
	video_print("\nebss:       ");
	video_print_hex(header->ebss, 0);
	video_print("\nstack_size: ");
	video_print_hex(header->stack_size, 0);

	video_print("\n");
}

static void halt(void) {
	while(1) {
		asm volatile ("hlt");
	}
}

static struct page gen_pml4e_pdpe_pte(void *addr)
{
	struct page page = {0};
	uint64_t ptr = (uint64_t)(uint32_t)addr;

	page.addr = (ptr >> 12);
	page.writ = 1;
	page.pres = 1;

	return page;
}

static struct page gen_pde(void *addr, uint64_t big)
{
	struct page page = {0};
	uint64_t ptr = (uint64_t)(uint32_t)addr;

	page.addr = (ptr >> 12);
	page.writ = 1;
	page.big = big;
	page.pres = 1;

	return page;
}

void main()
{
	struct mem_map *mem_map;
	struct kernel_header *header;
	uint32_t kernel_lba;
	uint32_t load_size;
	uint32_t load_size_sec;
	uint32_t total_size;
	uint32_t i;

	uint64_t buf;
	uint64_t vmem;
	uint64_t load_buf;
	uint64_t pages;
	uint64_t stack;
	struct page (*page_tab)[512];
	void *tmp;

	mem_map = (struct mem_map *)(rm_data.memmap_seg << 4);
	header = (struct kernel_header *)cdrom_buf;

	init_idt(&data_ready);
	video_init(rm_data.scr_width,
		   rm_data.scr_height,
		   UINT_TO_PTR(rm_data.framebuf_ptr),
		   UINT_TO_PTR(rm_data.font_ptr));
	video_clr();

	video_print("Memory detection successful\n");

	print_memmap(mem_map);

	video_print("Searching for kernel image...\n");
	if (cdrom_init(&data_ready, rm_data.boot_disk_io, rm_data.boot_disk_ctl,
		   rm_data.is_boot_disk_slv)) {
		video_print("Disk not supported :<\n");
		halt();
	}

	kernel_lba = cdrom_search(kernel_name, &load_size, cdrom_buf);
	if (kernel_lba == 0) {
		video_print("Kernel not found :<\n");
		halt();
	}

	video_print("Kernel image found at : ");
	video_print_hex(kernel_lba, 8);
	video_print(", size : ");
	video_print_hex(load_size, 8);
	video_print("\n");

	video_print("Loading kernel header...\n");
	cdrom_read_sector(kernel_lba, cdrom_buf);
	print_header(header);

	total_size = header->ebss - header->stext + header->stack_size;
	total_size += 10 * PAGE_SIZE;

	video_print("Total required memory size for kernel :");
	video_print_hex(total_size, 8);
	video_print("\n");

	buf = 0;
	total_size += PAGE_SIZE*2;
	for (i = 0; i < rm_data.memmap_size; i++) {
		if (mem_map[i].type != FREE_RAM) {
			continue;
		}

		if (mem_map[i].base < 0x100000) {
			continue;
		}

		if (mem_map[i].len >= total_size) {
			buf = PAGE_UP(mem_map[i].base);
			break;
		}
	}

	if (buf == 0) {
		video_print("Could not allocate sufficient memory :<\n");
		halt();
	}

	load_buf = buf;
	buf += PAGE_UP(header->ebss - header->stext);
	pages = buf;
	buf += 10 * PAGE_SIZE;
	stack = buf;
	buf += header->stack_size;
	vmem = BLOCK_UP(header->ebss);

	video_print("Video memory pointer : ");
	video_print_hex(vmem, 0);
	video_print("\n");
	video_print("Kernel pointer : ");
	video_print_hex(load_buf, 0);
	video_print("\n");
	video_print("Page structure pointer : ");
	video_print_hex(pages, 0);
	video_print("\n");
	video_print("Stack pointer : ");
	video_print_hex(stack, 0);
	video_print("\n");
	video_print("End pointer : ");
	video_print_hex(buf, 0);
	video_print("\n");

	for (i = pages; i < buf; i++) {
		*((uint8_t *)i) = 0x00;
	}

	page_tab = (struct page(*)[512])(uint32_t)pages;

	/* PML4E */
	page_tab[PML4E][511] = gen_pml4e_pdpe_pte(&page_tab[PDPE_T][0]);
	page_tab[0][256] = gen_pml4e_pdpe_pte(&page_tab[PDPE_M][0]);
	page_tab[PML4E][0] = gen_pml4e_pdpe_pte(&page_tab[PDPE_B][0]);

	/* PDPE */
	page_tab[PDPE_T][510] = gen_pml4e_pdpe_pte(&page_tab[PDE_KER][0]);
	page_tab[PDPE_T][511] = gen_pml4e_pdpe_pte(&page_tab[PDE_STA][0]);
	page_tab[PDPE_M][0] = gen_pml4e_pdpe_pte(&page_tab[PDE_MID][0]);
	page_tab[PDPE_B][0] = gen_pml4e_pdpe_pte(&page_tab[PDE_LOW][0]);

	/* PDE */
	page_tab[PDE_KER][0] = gen_pde(&page_tab[PTE_KER][0], 0);
	page_tab[PDE_LOW][0] = gen_pde(UINT_TO_PTR(0), 1);
	page_tab[PDE_STA][511] = gen_pde(&page_tab[PTE_STA][0], 0);

	/* PTE kernel + stack */
	for (i = 0; i < PAGE_CNT(pages - load_buf); i++) {
		tmp = UINT_TO_PTR((uint32_t)load_buf + (i * PAGE_SIZE));
		page_tab[PTE_KER][i] = gen_pml4e_pdpe_pte(tmp);
	}
	for (i = 511; i > 511 - PAGE_CNT(header->stack_size); i--) {
		tmp = UINT_TO_PTR((uint32_t)buf - ((511 - i) * PAGE_SIZE));
		page_tab[PTE_STA][i] = gen_pml4e_pdpe_pte(tmp);
	}

	load_size_sec = ((load_size + CD_SECTOR_SIZE - 1) / CD_SECTOR_SIZE);
	video_print("Loading : ");
	video_print_hex(load_size_sec, 0);
	video_print(" sectors\n");
	for (i = 0; i < load_size_sec; i++) {
		cdrom_read_sector(kernel_lba + i,
		(uint16_t *)((uint32_t)load_buf + (i * CD_SECTOR_SIZE)));
	}

	lm_data.framebuf_ptr = rm_data.framebuf_ptr;
	lm_data.font_ptr = rm_data.font_ptr;
	lm_data.memmap_ptr = (uint32_t)mem_map;
	lm_data.load_buf = load_buf;
	lm_data.peek_entry = (uint32_t)&page_tab[PDE_MID][0];
	lm_data.memmap_size = rm_data.memmap_size;
	lm_data.scr_width = rm_data.scr_width;
	lm_data.scr_height = rm_data.scr_height;
	lm_data.boot_disk_io = rm_data.boot_disk_io;
	lm_data.boot_disk_ctl = rm_data.boot_disk_ctl;
	lm_data.is_boot_disk_slv = rm_data.is_boot_disk_slv;
	lm_data.red_pos = rm_data.red_pos;
	lm_data.green_pos = rm_data.green_pos;
	lm_data.blue_pos = rm_data.blue_pos;
	lm_data.rsrvd_pos = rm_data.rsrvd_pos;

	run(header->start, page_tab, &lm_data);
}
