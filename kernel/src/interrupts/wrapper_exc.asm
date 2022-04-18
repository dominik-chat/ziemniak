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

%macro pushaq 0
push rax
push rbx
push rcx
push rdx
push rsi
push rdi
push rbp
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15
%endmacro

%macro exc_wrap_err 1
global exc_wrapper_%+%1
exc_wrapper_%+%1:
	pushaq
	mov rax, cr0
	push rax
	mov rax, cr2
	push rax
	mov rax, cr3
	push rax
	mov rax, cr4
	push rax
	mov rdi, %1
	mov rsi, rsp
	call exception_handler
	iretq
%endmacro

%macro exc_wrap_no_err 1
global exc_wrapper_%+%1
exc_wrapper_%+%1:
	push qword 0
	pushaq
	mov rax, cr0
	push rax
	mov rax, cr2
	push rax
	mov rax, cr3
	push rax
	mov rax, cr4
	push rax
	mov rdi, %1
	mov rsi, rsp
	call exception_handler
	iretq
%endmacro


exc_wrap_no_err 0
exc_wrap_no_err 1
exc_wrap_no_err 2
exc_wrap_no_err 3
exc_wrap_no_err 4
exc_wrap_no_err 5
exc_wrap_no_err 6
exc_wrap_no_err 7
exc_wrap_err    8
exc_wrap_no_err 9
exc_wrap_err    10
exc_wrap_err    11
exc_wrap_err    12
exc_wrap_err    13
exc_wrap_err    14
exc_wrap_no_err 15
exc_wrap_no_err 16
exc_wrap_err    17
exc_wrap_no_err 18
exc_wrap_no_err 19
exc_wrap_no_err 20
exc_wrap_no_err 21
exc_wrap_no_err 22
exc_wrap_no_err 23
exc_wrap_no_err 24
exc_wrap_no_err 25
exc_wrap_no_err 26
exc_wrap_no_err 27
exc_wrap_no_err 28
exc_wrap_no_err 29
exc_wrap_err    30
exc_wrap_no_err 31
