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

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

/**
 * @brief This function initializes IDT.
 *
 * The function initializes all exceptions to use an empty handler.
 * PIC is remapped to 32-48 positions.
 * Disk interrupts are the only interrupts exposed.
 *
 * @param drdy Pointer to data ready for disk interrupts.
 */
void init_idt(volatile uint8_t *drdy);

#endif
