.section .text.init

.global _start
.extern stack_top
.align 3

_start:
	la sp,stack_top
	call main
	