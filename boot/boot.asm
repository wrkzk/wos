extern kmain

MAGIC	 equ  0x1BADB002
FLAGS	 equ  0x0
CHECKSUM equ -MAGIC

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .bss
align 16
stack_bottom:
resb 16384
stack_top:

section .text
_start:
		mov esp, stack_top
		call kmain
		hlt

.end:
