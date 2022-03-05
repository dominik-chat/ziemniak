# Copyright (C) 2022 Dominik Chat
#
# This file is part of Ziemniak.
#
# Ziemniak is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 3
# of the License, or (at your option) any later version.
#
# Ziemniak is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Ziemniak. If not, see <https://www.gnu.org/licenses/>.

PRJ = stage1

ASMFLAGS = -f bin
ASM = nasm

all:
	@echo -e "\e[34mGenerating binary: $(PRJ).bin\e[0m"
	@mkdir -p build
	@$(ASM) $(PRJ).asm $(ASMFLAGS) -o build/$(PRJ).bin
	@echo -e "\e[33m\e[1mModule $(PRJ) built successfully\e[0m"

clean:
	@rm -rf build
