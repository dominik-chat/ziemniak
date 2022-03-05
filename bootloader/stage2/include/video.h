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

#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

/**
 * @brief This function initializes video driver.
 *
 * The function initializes driver local variables
 * according to provided data.
 *
 * @param width Width of video mode.
 * @param height Height of video mode.
 * @param vbuf Pointer to video memory buffer.
 * @param font Pointer to bios 8x16 font.
 */
void video_init(uint16_t width, uint16_t height, void *vbuf, void *font);

/**
 * @brief This function clears the screen.
 */
void video_clr(void);

/**
 * @brief Print NULL terminated string
 *
 * Print NULL terminated string. \n is only special char.
 * The function wraps in case of long string.
 *
 * @param text NULL terminated string pointer.
 */
void video_print(char *text);

/**
 * @brief Print value as hexadecimal string
 *
 * Print the hexadecimal representation
 * of provided value.
 * The hex string is shortened by offset
 * starintg from highest nibbles.
 *
 * @param val Value to print.
 * @param offset Offset of hex output in nibbles.
 */
void video_print_hex(uint64_t val, uint8_t offset);

/**
 * @brief Convert value to hex string
 *
 * Convert 64 bit value to hex string.
 * The output is NULL terminated, but does
 * not include '0x'
 *
 * @param val Value to convert.
 *
 * @retval Pointer to string buffer.
 */
char * video_convert_hex(uint64_t val);

#endif
