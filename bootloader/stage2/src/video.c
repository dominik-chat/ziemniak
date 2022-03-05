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

#include "video.h"
#include "defs.h"


#define FONT_SIZE_X	8
#define FONT_SIZE_Y	16


static uint16_t scr_width;
static uint16_t scr_height;
static volatile uint32_t *vid_buf;
static const uint8_t *bios_font;
static uint16_t text_col;
static uint16_t text_row;


static void setpixel(uint16_t x, uint16_t y, uint32_t val)
{
	uint32_t pos;

	pos = (y * scr_width) + x;

	vid_buf[pos] = val;
}

static void putpixel(uint16_t x, uint16_t y)
{
	setpixel(x, y, 0xFFFFFFFF);
}

static void clearpixel(uint16_t x, uint16_t y)
{
	setpixel(x, y, 0x00000000);
}

static void newline(void)
{
	text_col = 0;
	text_row++;

	if (text_row >= (scr_height / FONT_SIZE_Y)) {
		text_row = 0;
	}
}

static void putc(uint8_t chr)
{
	uint16_t x, y;
	uint16_t fnt_x, fnt_y;
	const uint8_t *glyph;

	glyph = &bios_font[chr * FONT_SIZE_Y];

	if (text_col >= (scr_width / FONT_SIZE_X)) {
		newline();
	}

	x = text_col * FONT_SIZE_X;
	y = text_row * FONT_SIZE_Y;

	for (fnt_y = 0; fnt_y < FONT_SIZE_Y; fnt_y++) {
		for (fnt_x = 0; fnt_x < FONT_SIZE_X; fnt_x++) {
			if (glyph[fnt_y] & (128 >> fnt_x)) {
				putpixel(x + fnt_x, y + fnt_y);
			} else {
				clearpixel(x + fnt_x, y + fnt_y);
			}
		}
	}

	text_col++;
}

void video_init(uint16_t width, uint16_t height, void *vbuf, void *font)
{
	scr_width = width;
	scr_height = height;
	vid_buf = vbuf;
	bios_font = font;
	text_col = 0;
	text_row = 0;
}

void video_clr(void)
{
	uint32_t i;

	for (i = 0; i < (scr_width * scr_height); i++) {
		vid_buf[i] = 0x00;
	}
}

void video_print(char *text)
{
	uint16_t i;

	i = 0;
	while (text[i] != '\0') {
		if (text[i] == '\n') {
			newline();
		} else {
			putc(text[i]);
		}

		i++;
	}
}

void video_print_hex(uint64_t val, uint8_t offset)
{
	if (offset < sizeof(val)*2) {
		video_print("0x");
		video_print(video_convert_hex(val)+offset);
		video_print(" ");
	}
}

char * video_convert_hex(uint64_t val)
{
	static char buf[17];
	uint8_t temp;
	uint8_t i;

	for (i = 0; i < 16; i++) {
		temp = ((val >> (4 * (15-i))) & 0x0F);

		if (temp < 0x0A) {
			buf[i] = temp + '0';
		} else{
			buf[i] = temp - 10 + 'A';
		}
	}

	buf[16] = '\0';
	return buf;
}














