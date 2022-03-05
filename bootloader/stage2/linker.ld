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

MEMORY
{
	MEM	(rwx) 	: ORIGIN = 0x00002000, LENGTH = 0x0008000
}

SECTIONS
{
	.init : 
	{
		*(.init)
	}  > MEM

	.text : 
	{
		. = ALIGN(4);
		*(.text)
		. = ALIGN(4);
	} > MEM

	.data :
	{
		. = ALIGN(4);
		*(.data)
		. = ALIGN(4);
	} > MEM

	.rodata :
	{
		. = ALIGN(4);
		*(.rodata)
		. = ALIGN(4);
	} > MEM

	.bss (NOLOAD) : 
	{
		. = ALIGN(4);
		_sbss = .;
		*(.COMMON)
		*(.bss)
		. = ALIGN(4);
		_ebss = .;
	} > MEM

	/DISCARD/  :
	{	
		*(.eh_frame)
		*(.comment)
	} 
}
