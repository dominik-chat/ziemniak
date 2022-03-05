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

#ifndef CDROM_H
#define CDROM_H

#include <stdint.h>

#define CD_SECTOR_SIZE	2048

/**
 * @brief This function initializes the ATAPI driver.
 *
 * The function initializes all local variables
 * and checks if the disk is supported
 *
 * @param drdy Pointer to a dara ready indicator.
 * @param disk_io Port number for disk io.
 * @param disk_ctl Port number for disk control
 * @param is_slv Non-negative for slave drive.
 *
 * @retval 0 No error - initialized successfully
 * @retval -1 Error - incorrect data or unsupported disk
 */
int cdrom_init(volatile uint8_t *drdy, uint16_t disk_io, uint16_t disk_ctl,
	       uint8_t is_slv);

/**
 * @brief This function reads a single sector.
 *
 * The function reads a single sector from ATAPI device
 * and stores it in a buffer. Buffer has to be 16-bit aligned.
 *
 * @param lba lba of sector to read.
 * @param buf pointer to read buffer.
 */
uint16_t cdrom_read_sector(uint32_t lba, uint16_t *buf);

/**
 * @brief This function searches main directory for a file.
 *
 * The function searches the main directory for a file with exact name.
 * The function returns the file size and file lba.
 *
 * @param fname file name.
 * @param fsize pointer to fize size variable.
 * @param buf pointer to read buffer.
 *
 * @retval 0, file not found
 * @retval >0 filesize in bytes
 */
uint32_t cdrom_search(const char *fname, uint32_t *fsize, uint16_t *buf);

#endif
