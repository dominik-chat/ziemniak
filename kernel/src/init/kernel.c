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
#include "memory/mm.h"
#include "debug/debug.h"
#include "defs.h"


__packed struct long_mode_data
{
	void		*framebuf_ptr;
	void		*font_ptr;
	void		*memmap_ptr;
	void		*load_buf;
	void		*load_end;
	void		*peek_entry;
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


static volatile uint32_t *vmem;


void main(struct long_mode_data *lm_data)
{
	size_t i;

	interrupts_init();
	debug_init();
	mm_init(lm_data->peek_entry, lm_data->memmap_ptr, lm_data->memmap_size,
					lm_data->load_buf, lm_data->load_end);

	debug_info("Kernel started");

	mm_peek((void **)&vmem, lm_data->framebuf_ptr,
				lm_data->scr_width * lm_data->scr_height * 4);

	for (i = 0; i < lm_data->scr_width * lm_data->scr_height; i++) {
	//	vmem[i] = 0x12345678;
	}

	while(1) {
	}
}
