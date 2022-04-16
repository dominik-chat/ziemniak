; Copyright (C) 2022 Dominik Chat
;
; This file is part of Ziemniak.
;
; Ziemniak is free software: you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation, either version 3
; of the License, or (at your option) any later version.
;
; Ziemniak is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
; See the GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with Ziemniak. If not, see <https://www.gnu.org/licenses/>.

[SECTION .text]
[BITS 64]


extern exception_handler
global isr_wrap_pic_table


%macro isr_wrap 1
extern isr_%+%1
global isr_wrapper_pic_%+%1
isr_wrapper_pic_%+%1:
	push rax
	call isr_%+%1
	mov al, 0x20
	out 0x20, al
	pop rax
	iretq
%endmacro

%macro isr_wrap_h 1
extern isr_%+%1
global isr_wrapper_pic_%+%1
isr_wrapper_pic_%+%1:
	push rax
	call isr_%+%1
	mov al, 0x20
	out 0xA0, al
	out 0x20, al
	pop rax
	iretq
%endmacro


isr_wrap 0
isr_wrap 1
isr_wrap 2
isr_wrap 3
isr_wrap 4
isr_wrap 5
isr_wrap 6
isr_wrap 7
isr_wrap_h 8
isr_wrap_h 9
isr_wrap_h 10
isr_wrap_h 11
isr_wrap_h 12
isr_wrap_h 13
isr_wrap_h 14
isr_wrap_h 15
