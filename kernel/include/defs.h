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

#ifndef _DEFS_H_
#define _DEFS_H_

#define __packed	__attribute__((packed))
#define __align(x)	__attribute__ ((aligned (x)))

#define UNUSED(x)		(void)(x)
#define UINT_TO_PTR(x)		((void *)(x))
#define PTR_TO_UINT(x)		((uint64_t)(x))
#define PAGE_UP(x)		(((x)+4095ull)&0xFFFFFFFFFFFFF000ull)
#define PAGE_DOWN(x)		((x)&0xFFFFFFFFFFFFF000ull)
#define PAGE_UP_PTR(x)		(UINT_TO_PTR(PAGE_UP(PTR_TO_UINT(x))))
#define PAGE_DOWN_PTR(x)	(UINT_TO_PTR(PAGE_DOWN(PTR_TO_UINT(x))))
#define PAGE_CNT(x)		(PAGE_UP(x)/PAGE_SIZE)
#define BLOCK_UP(x)		(((x)+2097151ull)&0xFFFFFFFFFFE00000ull)
#define BLOCK_DOWN(x)		((x)&0xFFFFFFFFFFE00000ull)
#define BLOCK_SIZE		2097152ull
#define PAGE_SIZE		4096ull

#endif /* _DEFS_H_ */
