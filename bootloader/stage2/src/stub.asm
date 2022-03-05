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
[BITS 16]

MEMMAP_SEG	equ	0x0A00
ID_BM		equ	(1 << 21)
LM_BM		equ	(1 << 29)
EDD_BUF_SIZE	equ	0x1E
EDD_SLV_BM	equ	(1 << 4)
VGA_FONT_DEST	equ	0x1000

global _start
global	rm_data
global	run
extern	_sbss
extern	_ebss
extern	main

magic_id:	dd	0xABCDEF12

_start:
	jmp init16

align 8
rm_data:
	rmd_framebuf_ptr:	dd	0x00
	rmd_font_ptr:		dd	VGA_FONT_DEST
	rmd_red_pos:		db	0x00
	rmd_green_pos:		db	0x00
	rmd_blue_pos:		db	0x00
	rmd_rsrvd_pos:		db	0x00
	rmd_memmap_seg:		dw	0x00
	rmd_memmap_size:	dw	0x00
	rmd_scr_width:		dw	0x00
	rmd_scr_height:		dw	0x00
	rmd_boot_disk:		db	0x00
	rmd_boot_disk_slv:	db	0x00
	rmd_boot_disk_io:	dw	0x00
	rmd_boot_disk_ctl:	dw	0x00


	screen_pixels:		dd	0x00
	screen_mode:		dw	0x00

align 4
	edd_buf:	times	EDD_BUF_SIZE	db	0x00

init16:
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov [rmd_boot_disk], dl

id_drive:
	mov ah, 0x48
	mov word [edd_buf], EDD_BUF_SIZE
	mov si, edd_buf
	int 0x13
	jc .id_drive_fail
	cmp ah, 0x00
	jnz .id_drive_fail
	cmp word [edd_buf], EDD_BUF_SIZE
	jne .id_drive_fail
	mov si, [ds:edd_buf+0x1A]
	mov ax, [ds:edd_buf+0x1C]
	mov ds, ax
	lodsw
	mov [es:rmd_boot_disk_io], ax
	lodsw
	mov [es:rmd_boot_disk_ctl], ax
	lodsb
	and al, EDD_SLV_BM
	mov [ds:rmd_boot_disk_slv], al
	xor ax, ax
	mov ds, ax
	jmp check_cpuid

.id_drive_fail:
	mov ax, ERR_MSG_ID_DRIVE_FAIL
	jmp boot_fail16

check_cpuid:
;save old eflags
	pushfd
;try to invert id bit
	pushfd
	xor dword [esp], ID_BM
	popfd
;load new eflags
	pushfd
	pop eax
	pop ebx
	xor eax, ebx
	test eax, ID_BM
	jnz check_ex_cpuid
;no cpuid
	mov ax, ERR_MSG_NO_CPUID
	jmp boot_fail16

check_ex_cpuid:
	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001
	jnb check_long_mode
;no extended long mode
	mov ax, ERR_MSG_NOT_64BIT
	jmp boot_fail16

check_long_mode:
	mov eax, 0x80000001
	cpuid
	test edx, LM_BM
	jnz get_font
;no long mode :<
	mov ax, ERR_MSG_NOT_64BIT
	jmp boot_fail16

get_font:
	push ds
	push es
	mov ax, 0x1130
	mov bh, 0x06
	int 0x10
	push es
	pop ds
	pop es
	mov si, bp
	mov di, VGA_FONT_DEST
	mov cx, 256*16
	rep movsb
	pop ds

svga_init:
;use memmap_seg as buffer
	mov ax, MEMMAP_SEG
	mov [ds:rmd_memmap_seg], ax

;list all possible modes
	mov es, ax
	xor di, di
	mov ax, 0x4F00
	int 0x10

;find biggest supported mode
findVBEmode:
	mov al, [es:0x05]
	cmp al, 0x02
	jb .legacyVBE
	
	mov word ax, [es:0x0E]
	mov si, ax ;mode ptr
	mov word ax, [es:0x10]
	mov fs, ax ;mode ptr segment
	
	;ds:screen_pixels best pixel count
	;ds:screen_mode best mode number
	add di, 512 ;free buffer
.getMaximum:
	mov word cx, [fs:si]
	or cx, 0x4000 ;linear framebuffer
	mov ax, 0x4F01
	int 0x10
;check for 32 bpp
	mov byte al, [es:di+0x19]
	cmp al, 32
	jne .checknext
;check if supported
	mov word ax, [es:di+0x00]
	test ax, 0x0001
	jz .checknext
;bpp is correct, and mode is supported - check resolution
	mov edx, 0x87650000
	mov ax, [es:di+0x12] ;width
	mov bx, [es:di+0x14] ;height

;CHECK FOR VM - I WANT MY MACHINE TO NOT BE AS BIG AS POSSIBLE
	cmp bx, 1000
	jae .checknext
;CHECK FOR VM - I WANT MY MACHINE TO NOT BE AS BIG AS POSSIBLE

	mul bx
	shl edx, 16
	mov dx, ax
	cmp edx, [ds:screen_pixels]
	jbe .checknext
	mov word cx, [fs:si]
	or cx, 0x4000 ;linear framebuffer
	mov word [ds:screen_mode], cx
	mov dword [ds:screen_pixels], edx
	
.checknext:
	add si, 2
	cmp word [fs:si], 0xFFFF
	jne .getMaximum
	mov cx, word [ds:screen_mode]
	jmp .SetVBE
	
.legacyVBE:
	mov cx, 0x4115 ;800x600 24bpp
	mov word [ds:screen_mode], cx
.SetVBE:
	xor di, di
	mov ax, 0x4F01
	int 0x10
;test for support or fail
	cmp ah, 0x00
	jne .svga_fail
	cmp al, 0x4F
	jne .svga_fail
	
;save framebuffer address
	mov dword ecx, [es:0x28]
	mov dword [ds:rmd_framebuf_ptr], ecx
;save rgb positions
	mov byte cl, [es:0x20]
	mov byte [ds:rmd_red_pos], cl
	mov byte cl, [es:0x22]
	mov byte [ds:rmd_green_pos], cl
	mov byte cl, [es:0x24]
	mov byte [ds:rmd_blue_pos], cl
	mov byte cl, [es:0x26]
	mov byte [ds:rmd_rsrvd_pos], cl
;save screen size positions
	mov word cx, [es:0x12]
	mov word [ds:rmd_scr_width], cx
	mov word cx, [es:0x14]
	mov word [ds:rmd_scr_height], cx
	
	xor di, di
	mov ax, 0x4F02
	mov word bx, [ds:screen_mode]
	int 0x10
;test for support or fail
	cmp ah, 0x00
	jne .svga_fail
	cmp al, 0x4F
	jne .svga_fail
	
;all good, go map memory
	jmp map_memory

.svga_fail:
	mov ax, ERR_MSG_SVGA_FAIL
	jmp boot_fail16

map_memory:
	xor di, di
	mov ax, MEMMAP_SEG
	mov es, ax
	call e820
	jnc save_memmap
	mov ax, ERR_MSG_E820_FAIL
	jmp boot_fail16

;first iteration is different (first carry means fail)
e820:
	xor bp, bp
	xor ebx, ebx
	mov edx, 0x0534D4150
	mov eax, 0xe820
	mov dword [es:di+20], 1
	mov ecx, 24
	int 0x15
	jc .fail
	jmp .first
;if carry in loop - end of map
.loop:
	mov eax, 0xe820
	mov dword [es:di+20], 1
	mov ecx, 24
	int 0x15
	jc .end
.first:
	mov edx, 0x0534D4150
	cmp edx, eax
	jne .fail
	jcxz .skip
	cmp ecx, 20
	jbe .noacpi
	test byte [es:di+20], 1
	je .skip
.noacpi:
	mov ecx, [es:di+8]
	add ecx, [es:di+12]
	jz .skip
	inc bp
	add di, 24
;ignore some entries (eiter empty or invalid)
.skip:
	cmp ebx, 0
	jne .loop
.end:
	clc
	ret
.fail:
	stc
	ret

save_memmap:
	mov word [ds:rmd_memmap_size], bp

activate_a20:
	in al, 0x92
	or al, 2
	out 0x92, al
	
	call check_a20
	cmp ax, 0x01
	jne .a20_failed
	jmp load_gdt

.a20_failed:
	mov ax, ERR_MSG_A20_FAIL
	jmp boot_fail16

check_a20:
    pushf
    push ds
    push es
    push di
    push si
	
    xor ax, ax ; ax = 0
    mov es, ax
    not ax ; ax = 0xFFFF
    mov ds, ax
    mov di, 0x0500
    mov si, 0x0510
    mov al, byte [es:di]
    push ax
    mov al, byte [ds:si]
    push ax
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF
    cmp byte [es:di], 0xFF
    pop ax
    mov byte [ds:si], al
    pop ax
    mov byte [es:di], al
    mov ax, 0
    je .check_a20_exit
    mov ax, 1
 
.check_a20_exit:
    pop si
    pop di
    pop es
    pop ds
    popf
    ret

load_gdt:                  
	cli
	lgdt [gdtr]

	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp 0x08:code32

boot_fail16:
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
;forever loop
	jmp .done

ERR_MSG_LOAD_BOOT: 	db "CD LOAD FAILED",0
ERR_MSG_SVGA_FAIL: 	db "SVGA MODE NOT SUPPORTED",0
ERR_MSG_E820_FAIL:	db "E820 FUNCTION NOT SUPPORTED",0
ERR_MSG_A20_FAIL: 	db "A20 GATE FAILED TO INITIALIZE",0
ERR_MSG_NO_CPUID:	db "CPUID NOT SUPPORTED",0
ERR_MSG_NOT_64BIT:	db "CPU IS NOT 64-BIT",0
ERR_MSG_ID_DRIVE_FAIL:	db "BOOT DRIVE IDENTIFICATION FAILED",0

[BITS 32]
[SECTION .init32]
code32:

	mov ax, 0x10
	mov ds, eax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov eax, 0x7FF0
	shl eax, 4
	mov esp, eax
	mov ebp, esp

	cld
	mov ecx, _ebss
	sub ecx, _sbss
	xor al, al
	mov edi, _sbss
	rep stosb

	call main

run:
	cli
	mov eax, 0xA0
	mov cr4, eax
	mov edx, [esp+12]
	mov cr3, edx
	mov ecx, 0xC0000080
	rdmsr
	or eax, (1 << 8)
	wrmsr
	mov ebx, cr0
	or ebx, (1 << 31)
	mov cr0, ebx
	lgdt [gdtr_64]

	jmp 0x08:long_mode

[BITS 64]
long_mode:
	mov rbx, [esp+4]
	xor rcx, rcx
	mov ecx, [esp+16]
	mov rax, 0xFFFFFFFFFFFFFFF0
	mov rsp, rax
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov rax, rcx
	jmp rbx

align 4
gdtr_64:
	dw gdt_64_end-gdt_64_start-1
	dd gdt_64_start

gdt_64_start:
	dq 0x0000000000000000             ; Null Descriptor - should be present.

gdt_64_code:
	dq 0x00209A0000000000             ; 64-bit code descriptor (exec/read).

gdt_64_data:
	dq 0x0000920000000000             ; 64-bit data descriptor (read/write).

gdt_64_end:



[BITS 32]
[SECTION .init_data]
align 4
gdtr:
    dw gdt_end-gdt_start-1
    dd gdt_start

gdt_start:
    ; First entry is always the Null Descriptor
    dd 0
    dd 0

gdt_code:
    ; 4gb flat r/w/executable code descriptor
    dw 0xFFFF           ; limit low
    dw 0                ; base low
    db 0                ; base middle
    db 0b10011010       ; access
    db 0b11001111       ; granularity
    db 0                ; base high

gdt_data:
    ; 4gb flat r/w data descriptor
    dw 0xFFFF           ; limit low
    dw 0                ; base low
    db 0                ; base middle
    db 0b10010010       ; access
    db 0b11001111       ; granularity
    db 0                ; base high
	
gdt_end:
