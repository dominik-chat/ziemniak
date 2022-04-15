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
global tss
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

init_gdt:
	mov rax, tss
	mov word [basel], ax
	shr rax, 16
	mov byte [baseh], al
	shr rax, 8
	mov byte [baseh+3], al
	lgdt [gdtr]
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax
	jmp far [call_addr]

call_main:
	pop rdi
	mov rbp, rsp
	call main

loop:
	hlt
	jmp loop

[SECTION .data]
align 4
tss:
	times 0x66 db 0
	dw 0x68

[SECTION .rodata]
align 8
call_addr:
	dq call_main
	dw 0x08

align 8
gdtr:
	dw gdt_end-gdt_null-1
	dq gdt_null

gdt_null:
	dq 0x0000000000000000

gdt_code:
	dq 0x0020980000000000

gdt_data:
	dq 0x0000920000000000

gdt_tss:
	dw 0x00000068
basel:	dw 0x0000
baseh: dd 0x11008900
	dd 0xFFFFFFFF
	dd 0x00000000

gdt_end:
