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

#include <stddef.h>


typedef void (*serial_rx_t)(void);


/**
 * @brief Initialize serial port.
 *
 * Initialize serial port in blocking (tx) mode.
 *
 * @param id Number of serial port to initialize.
 * @param rx_cb Callback to serial receive event.
 *
 * @retval 0 Success.
 * @retval EIO Port not present.
 * @retval EINVAL Invalid argument.
 */
int serial_init(size_t id, serial_rx_t rx_cb);

#endif /* _DEBUG_H_ */
