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

#ifndef _MM_H_
#define _MM_H_

#include <stdint.h>
#include <stddef.h>


/**
 * @brief Initialize memory manager.
 *
 * Initialize memory manager.
 *
 * @param peek_entry Pointer to PTE entry used for peeking.
 * @param memmap Pointer to memory map by e820.
 * @param memmap_size Size of memmap (number of entries).
 * @param load_buf Pointer to kernel load buffer.
 * @param load_end Pointer to end of load buffer.
 */
void mm_init(void *peek_entry,
	     void *memmap,
	     size_t memmap_size,
	     void *load_buf,
	     void *load_end);

/**
 * @brief Peek memory.
 *
 * Map physical memory to virtual memory with a given size.
 * Maximum size is 1GB.
 *
 * @param virt Pointer to a pointer to save virtual memory address.
 * @param phys Pointer to requested physical memory.
 * @param size Size of requested peek.
 *
 * @retval EPERM MM not initialized.
 * @retval EINVAL Invalid input arguments.
 * @retval ENOTSUP Size too big or out of bounds.
 */
int mm_peek(void **virt, void *phys, size_t size);

/**
 * @brief Generate cr3 value and set it.
 *
 * Generate cr3 value with given parameters and set it.
 *
 * @param pml4 Physical pointer to pml4.
 * @param flags Flags for cr3.
 */
void mm_set_cr3(void *pml4, uint64_t flags);

#endif /* _MM_H_ */
