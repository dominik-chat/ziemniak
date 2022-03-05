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

#include "cdrom.h"


#define PATAPI_ID	0xEB14
#define SATAPI_ID	0x9669
#define	PATA_ID		0x0000
#define SATA_ID		0xC33C

#define PVD_START	(0x10)
#define PVD_ID		(0x01)
#define PVD_SIGNATURE	(0x01)

#define DIR_OFFSET	(0x9C)
#define DIR_SIZE	(0x00)
#define DIR_EXT_LOC	(0x02)
#define DIR_EXT_SIZE	(0x0A)
#define DIR_FILENAME	(0x21)


static volatile uint8_t *data_ready;
static uint16_t io_port;
static uint16_t ctl_port;
static uint8_t is_slave;


static inline void outb(uint16_t port, uint8_t val)
{
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

static inline void outsw(uint16_t port, void *addr, uint32_t count)
{
	asm volatile("rep; outsw" : "+S"(addr), "+c"(count) : "d"(port));
}

static inline void insw(uint16_t port, void *addr, uint32_t count)
{
	asm volatile("rep; insw" : "+D"(addr), "+c"(count) : "d"(port));
}

static void cdrom_soft_reset(void)
{
	uint8_t sts;

	outb(ctl_port, 0x04);
	outb(ctl_port, 0x00);

	inb(ctl_port);
	inb(ctl_port);
	inb(ctl_port);
	inb(ctl_port);

	sts = inb(ctl_port);

	while((sts & 0xC0) == 0x80)
	{
		sts = inb(ctl_port);
	}

}

static int compare_str(const char *str, void *mem)
{
	int i;
	char *temp;

	temp = mem;

	for (i = 0; str[i] != 0; i++) {
		if (str[i] != temp[i]) {
			return (i + 1);
		}

		if (i == 256) {
			return i;
		}
	}

	return 0;
}

static uint32_t search_directory(uint32_t *fsize, uint8_t *buf,
							const char *fname)
{
	uint32_t offset;

	offset = 0;
	while (buf[DIR_SIZE + offset] != 0x00) {
		if (compare_str(fname, &buf[DIR_FILENAME + offset]) == 0) {
			*fsize = (*((uint32_t *)&buf[DIR_EXT_SIZE + offset]));
			return (*((uint32_t *)&buf[DIR_EXT_LOC + offset]));
		}
		offset += buf[DIR_SIZE + offset];
	}

	return 0;
}

int cdrom_init(volatile uint8_t *drdy, uint16_t disk_io, uint16_t disk_ctl,
	       uint8_t is_slv)
{
	uint8_t sig_low;
	uint8_t sig_high;

	data_ready = drdy;
	io_port = disk_io;
	ctl_port = disk_ctl;

	if (is_slv != 0) {
		is_slave = 1;
	} else {
		is_slave = 0;
	}

	cdrom_soft_reset();
	outb((io_port + 0x06), (0xA0 | (is_slave << 4)));

	inb(ctl_port);
	inb(ctl_port);
	inb(ctl_port);
	inb(ctl_port);

	sig_low = inb(io_port + 0x04);
	sig_high = inb(io_port + 0x05);

	if (((sig_high << 8) | sig_low) != PATAPI_ID) {
		return -1;
	}

	return 0;
}

uint16_t cdrom_read_sector(uint32_t lba, uint16_t *buf)
{
	uint8_t read_cmd[12] = { 0x28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint16_t rd_size;
	uint8_t sts;

	*data_ready = 0;
	outb((io_port + 0x06), ((is_slave)<<4));

	//delay 400n
	inb(ctl_port);
	inb(ctl_port);
	inb(ctl_port);
	inb(ctl_port);

	outb((io_port + 0x01), 0x00); //PIO
	outb((io_port + 0x04), (CD_SECTOR_SIZE & 0xFF)); //size
	outb((io_port + 0x05), (CD_SECTOR_SIZE >> 8));
	outb((io_port + 0x07), 0xA0); //ata packet command

	while ((inb((io_port + 0x07))) & 0x80) {
		asm volatile ("pause");
	}

	while (!((sts = inb((io_port + 0x07))) & 0x8) && !(sts & 0x1)) {
		asm volatile ("pause");
	}

	if (sts & 0x01) {
		return 0;
	}

	read_cmd[8] = 1;
	read_cmd[2] = (lba >> 0x18) & 0xFF;
	read_cmd[3] = (lba >> 0x10) & 0xFF;
	read_cmd[4] = (lba >> 0x08) & 0xFF;
	read_cmd[5] = (lba >> 0x00) & 0xFF;
	outsw (io_port, (uint16_t *)read_cmd, 6);

	while(*data_ready == 0);

	rd_size = ((((uint16_t)inb((io_port + 0x05))) << 8) |
						(inb((io_port + 0x04))));

	insw((io_port), buf, (rd_size / 2));

	while ((sts = inb(io_port + 0x07)) & 0x88) {
		asm volatile ("pause");
	}

	return rd_size;
}

uint32_t cdrom_search(const char *fname, uint32_t *fsize, uint16_t *buf)
{
	uint32_t cur_dir_lba;
	uint32_t lba_off;
	uint32_t dir_lba;
	uint32_t dir_size;
	uint16_t dir_sectors;
	uint32_t temp;
	uint8_t *buff;

	buff = (uint8_t *)buf;

	lba_off = 0;
	while(buff[0] != PVD_SIGNATURE)
	{
		cdrom_read_sector(PVD_START + lba_off, (uint16_t *)buff);
		lba_off++;
	}

	dir_lba = *((uint32_t *)&buff[DIR_OFFSET+DIR_EXT_LOC]);
	dir_size = *((uint32_t *)&buff[DIR_OFFSET+DIR_EXT_SIZE]);

	dir_sectors = dir_size/CD_SECTOR_SIZE;

	for (cur_dir_lba = 0; cur_dir_lba < (dir_sectors); cur_dir_lba++) {
		cdrom_read_sector(dir_lba+cur_dir_lba, (uint16_t *)buff);

		temp = search_directory(fsize, buff, fname);
		if (temp != 0) {
			return temp;
		}
	}

	*fsize = 0x00;
	return 0x00;
}
