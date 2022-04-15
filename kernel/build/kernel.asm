
build/kernel.elf:     file format elf64-x86-64


Disassembly of section .text:

ffffffff80000000 <_stext>:
ffffffff80000000:	50                   	push   %rax
ffffffff80000001:	00 00                	add    %al,(%rax)
ffffffff80000003:	80 ff ff             	cmp    $0xff,%bh
ffffffff80000006:	ff                   	(bad)  
ffffffff80000007:	ff                 	incl   (%rax)

ffffffff80000008 <kernel_table.stext>:
ffffffff80000008:	00 00                	add    %al,(%rax)
ffffffff8000000a:	00 80 ff ff ff ff    	add    %al,-0x1(%rax)

ffffffff80000010 <kernel_table.etext>:
ffffffff80000010:	9c                   	pushf  
ffffffff80000011:	00 00                	add    %al,(%rax)
ffffffff80000013:	80 ff ff             	cmp    $0xff,%bh
ffffffff80000016:	ff                   	(bad)  
ffffffff80000017:	ff                 	incl   (%rax)

ffffffff80000018 <kernel_table.sdata>:
ffffffff80000018:	00 10                	add    %dl,(%rax)
ffffffff8000001a:	00 80 ff ff ff ff    	add    %al,-0x1(%rax)

ffffffff80000020 <kernel_table.edata>:
ffffffff80000020:	00 10                	add    %dl,(%rax)
ffffffff80000022:	00 80 ff ff ff ff    	add    %al,-0x1(%rax)

ffffffff80000028 <kernel_table.srodata>:
ffffffff80000028:	00 10                	add    %dl,(%rax)
ffffffff8000002a:	00 80 ff ff ff ff    	add    %al,-0x1(%rax)

ffffffff80000030 <kernel_table.erodata>:
ffffffff80000030:	00 10                	add    %dl,(%rax)
ffffffff80000032:	00 80 ff ff ff ff    	add    %al,-0x1(%rax)

ffffffff80000038 <kernel_table.sbss>:
ffffffff80000038:	00 10                	add    %dl,(%rax)
ffffffff8000003a:	00 80 ff ff ff ff    	add    %al,-0x1(%rax)

ffffffff80000040 <kernel_table.ebss>:
ffffffff80000040:	00 10                	add    %dl,(%rax)
ffffffff80000042:	00 80 ff ff ff ff    	add    %al,-0x1(%rax)

ffffffff80000048 <kernel_table.stacksize>:
ffffffff80000048:	00 00                	add    %al,(%rax)
ffffffff8000004a:	01 00                	add    %eax,(%rax)
ffffffff8000004c:	00 00                	add    %al,(%rax)
	...

ffffffff80000050 <_start>:
ffffffff80000050:	50                   	push   %rax
ffffffff80000051:	fc                   	cld    
ffffffff80000052:	48 31 c0             	xor    %rax,%rax
ffffffff80000055:	48 b9 00 10 00 80 ff 	movabs $0xffffffff80001000,%rcx
ffffffff8000005c:	ff ff ff 
ffffffff8000005f:	48 81 e9 00 10 00 80 	sub    $0xffffffff80001000,%rcx
ffffffff80000066:	48 c1 e9 03          	shr    $0x3,%rcx
ffffffff8000006a:	48 bf 00 10 00 80 ff 	movabs $0xffffffff80001000,%rdi
ffffffff80000071:	ff ff ff 
ffffffff80000074:	f3 48 ab             	rep stos %rax,%es:(%rdi)

ffffffff80000077 <init_sse>:
ffffffff80000077:	0f 20 c0             	mov    %cr0,%rax
ffffffff8000007a:	83 e0 fb             	and    $0xfffffffb,%eax
ffffffff8000007d:	48 83 c8 02          	or     $0x2,%rax
ffffffff80000081:	0f 22 c0             	mov    %rax,%cr0
ffffffff80000084:	0f 20 e0             	mov    %cr4,%rax
ffffffff80000087:	48 0d 00 06 00 00    	or     $0x600,%rax
ffffffff8000008d:	0f 22 e0             	mov    %rax,%cr4
ffffffff80000090:	5f                   	pop    %rdi
ffffffff80000091:	48 89 e5             	mov    %rsp,%rbp
ffffffff80000094:	e8 07 00 00 00       	call   ffffffff800000a0 <main>

ffffffff80000099 <loop>:
ffffffff80000099:	f4                   	hlt    
ffffffff8000009a:	eb fd                	jmp    ffffffff80000099 <loop>

Disassembly of section .text.startup:

ffffffff800000a0 <main>:
ffffffff800000a0:	0f b7 4f 1a          	movzwl 0x1a(%rdi),%ecx
ffffffff800000a4:	0f b7 57 1c          	movzwl 0x1c(%rdi),%edx
ffffffff800000a8:	48 8b 07             	mov    (%rdi),%rax
ffffffff800000ab:	0f af d1             	imul   %ecx,%edx
ffffffff800000ae:	48 63 ca             	movslq %edx,%rcx
ffffffff800000b1:	85 d2                	test   %edx,%edx
ffffffff800000b3:	74 1a                	je     ffffffff800000cf <main+0x2f>
ffffffff800000b5:	48 8d 14 88          	lea    (%rax,%rcx,4),%rdx
ffffffff800000b9:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
ffffffff800000c0:	c7 00 78 56 34 12    	movl   $0x12345678,(%rax)
ffffffff800000c6:	48 83 c0 04          	add    $0x4,%rax
ffffffff800000ca:	48 39 d0             	cmp    %rdx,%rax
ffffffff800000cd:	75 f1                	jne    ffffffff800000c0 <main+0x20>
ffffffff800000cf:	eb fe                	jmp    ffffffff800000cf <main+0x2f>
