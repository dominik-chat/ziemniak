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

PRJ = bootloader

BUILD_DIR = build

all: prepare submodules end

prepare:
	@echo -e "\e[33m\e[1mBuilding $(PRJ) module\e[0m"
	@mkdir -p $(BUILD_DIR)

submodules:
	@cd stage1 && $(MAKE) all
	@cd stage2 && $(MAKE) all

end:
	@echo -e "\e[33m\e[1mModule $(PRJ) built successfully\e[0m"
	@cp stage1/$(BUILD_DIR)/stage1.bin $(BUILD_DIR)/stage1.bin
	@cp stage2/$(BUILD_DIR)/stage2.bin $(BUILD_DIR)/stage2.bin

clean:
	@cd stage1 && $(MAKE) clean
	@cd stage2 && $(MAKE) clean
	@rm -rf $(BUILD_DIR)
