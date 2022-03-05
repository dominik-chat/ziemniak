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

ENTRY(_start);

_stacksize = 64K;

MEMORY
{
	LMEM	(rwx) 	: ORIGIN = 0xFFFFFFFF80000000, LENGTH = 0x200000
}

SECTIONS
{
	.text : ALIGN(4K)
	{
		_stext = .;
		*(.init)
		*(.text)
		_etext = .;
	} > LMEM

	.data : ALIGN(4K)
	{
		_sdata = .;
		*(.data)
		_edata = .;
	} > LMEM

	.rodata : ALIGN(4K)
	{
		_srodata = .;
		*(.rodata)
		_erodata = .;
	} > LMEM

	.bss (NOLOAD) : ALIGN(4K)
	{
		_sbss = .;
		*(.COMMON)
		*(.bss)
		. = ALIGN(4K);
		_ebss = .;
	} > LMEM

	/DISCARD/  :
	{	
		*(.eh_frame)
		*(.comment)
	} 
}
