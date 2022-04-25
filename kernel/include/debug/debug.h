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

#ifndef _DEBUG_H_
#define _DEBUG_H_


/**
 * @brief Initialize debug interface.
 *
 * Initialize serial port 0.
 *
 * @retval 0 Success.
 * @retval EIO Port not present.
 * @retval EINVAL Invalid argument in function.
 */
int debug_init(void);

/**
 * @brief Print debug info message.
 *
 * Print info message via initialized debug interface.
 *
 * @param msg Message to print.
 */
void debug_info(const char *msg);

/**
 * @brief Print debug info message.
 *
 * Print warning message via initialized debug interface.
 *
 * @param msg Message to print.
 */
void debug_warn(const char *msg);

/**
 * @brief Print debug info message.
 *
 * Print error message via initialized debug interface.
 *
 * @param msg Message to print.
 */
void debug_err(const char *msg);

/**
 * @brief Print debug number as hex.
 *
 * Print hexadecimal representation of a number.
 *
 * @param msg Message to print before number.
 * @param num Number to print.
 */
void debug_num(const char *msg, uint64_t num);

/**
 * @brief Print debug crash message.
 *
 * Print crash message via initialized debug interface.
 *
 * @param excep Exception number.
 * @param dump Pointer to dumped core data.
 */
void debug_crash(int excep, void *dump);

#endif /* _DEBUG_H_ */
