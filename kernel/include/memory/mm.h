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
 * @typedef allocator_get_req_t
 * @brief API for getting the allocators memory requirements.
 *
 * @param max_addr Maximum physical address.
 *
 * @retval Amount of memory to allocate for allocator (in bytes).
 */
typedef size_t (*allocator_get_req_t)(uint64_t max_addr);

/**
 * @typedef allocator_init_t
 * @brief API for initializing the allocator.
 *
 * @param buf Memory buffer for the allocator.
 *
 * @retval 0 Initialization successful.
 */
typedef int (*allocator_init_t)(void *buf);

/**
 * @typedef allocator_alloc_t
 * @brief API for requesting memory allocation.
 *
 * @param buf Pointer to allocated memory.
 * @param len Size of allocation (in bytes).
 *
 * @retval 0 Allocation successful.
 * @retval ENOMEM Out of memory.
 */
typedef int (*allocator_alloc_t)(void **buf, size_t len);

/**
 * @typedef allocator_free_t
 * @brief API for initializing the allocator.
 *
 * @param buf Pointer to allocated memory.
 * @param len Size of allocation (in bytes).
 *
 * @retval 0 Free successful.
 * @retval EINVAL Invalid parameters.
 */
typedef int (*allocator_free_t)(void *buf, size_t len);


struct allocator_api {
	allocator_get_req_t get_req;
	allocator_init_t init;
	allocator_alloc_t alloc;
	allocator_free_t free;
};


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
