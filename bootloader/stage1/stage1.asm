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

[ORG 0x0800]

[SECTION .text]
[BITS 16]

STACK_SEG	equ	0x7000
STAGE1_SRC_SEG	equ	0x07C0
STAGE1_DST_SEG	equ	0x0080
CD_SECT_SIZE	equ	0x0800
STAGE2_SEC_CNT	equ	0x0010
STAGE2_ADDR	equ	0x2004
STAGE2_SEG	equ	0x0200

_start:
	jmp short start

;extended read disk address packet
align 8
disk_packet:
	da_PacketSize	db		0x10	;size of packet
	da_Reserved	db		0x00	;reserved always 0
	da_BlockCount	dw		STAGE2_SEC_CNT	;number of blocks to transfer
	da_BufferOff	dw		0
	da_BufferSeg	dw		STAGE2_SEG
	da_LBA_number	dq		0	;LBA

disk_number:	db	0

start:
;stack at top of 640kb memory, 64kb free space
	cli
	mov ax, STACK_SEG
	mov ss, ax
	mov sp, 0xFFF0
	sti
	cld

;ds holds Source, es Destination
	mov ax, STAGE1_SRC_SEG
	mov ds, ax
	mov ax, STAGE1_DST_SEG
	mov es, ax

;copy Stage1 to lower location (lowest mem)
	xor si, si
	xor di, di
	mov cx, CD_SECT_SIZE
	rep movsb
	
;store drive number
	push dx

;jump to set proper code segment
	jmp 0x0000:load_boot

;load stage2
load_boot:
;check for edd
	mov bx, 0x55AA
	mov ah, 0x41
	int 0x13
	jc .edd_nsup
	cmp bx, 0xAA55
	jnz .edd_nsup
	test cx, 0x03
	jz .edd_nsup
	cmp ah, 0x30
	jnz .edd_nsup
	jmp edd_supp
.edd_nsup:
	mov ax, ERR_MSG_EDD_NSUP
	jmp read_fail
edd_supp:
;load from CD
	xor ax, ax
	mov ds, ax
	mov es, ax

	mov ah, 0x42
	mov si, disk_packet
	int 0x13
	jnc .load_boot_success

;read failed
	mov ax, ERR_MSG_CD_READ
	jmp read_fail

;success jump to stage2
.load_boot_success:
;restore drive number
	pop dx
	jmp 0x0000:STAGE2_ADDR

read_fail:
	mov si, ax
	mov ah, 0x00
	mov al, 0x03
	int 0x10
.print_loop:
	lodsb
	or al, al
	jz .done
	mov ah, 0x0E
	int 0x10
	jmp .print_loop
.done:
	jmp .done

ERR_MSG_CD_READ:	db "CD LOAD FAILED",0
ERR_MSG_EDD_NSUP:	db "EDD NOT SUPPORTED",0

times (CD_SECT_SIZE-($-$$)) db 0
