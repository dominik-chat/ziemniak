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

[SECTION .init]
[BITS 64]

global _start
extern	main

extern _stext;
extern _etext;
extern _sdata;
extern _edata;
extern _srodata;
extern _erodata;
extern _sbss
extern _ebss
extern _stacksize

kernel_table:
	.start:		dq	_start
	.stext:		dq	_stext
	.etext:		dq	_etext
	.sdata:		dq	_sdata
	.edata:		dq	_edata
	.srodata:	dq	_srodata
	.erodata:	dq	_erodata
	.sbss:		dq	_sbss
	.ebss:		dq	_ebss
	.stacksize:	dq	_stacksize

;rax - long mode data
_start:
	push rax

	cld
	xor rax, rax
	mov rcx, _ebss
	sub rcx, _sbss
	shr rcx, 3
	mov rdi, _sbss
	rep stosq

init_sse:
	mov rax, cr0
	and eax, 0xFFFFFFFB
	or rax, 0x02
	mov cr0, rax

	mov rax, cr4
	or rax, (3 << 9)
	mov cr4, rax

	pop rdi
	mov rbp, rsp
	call main

loop:
	hlt
	jmp loop
