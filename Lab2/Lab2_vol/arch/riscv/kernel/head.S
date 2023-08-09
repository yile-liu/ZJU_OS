.section .text.init

.global _start
.extern stack_top
.extern start_kernel
.extern bss_start
.extern bss_end
.extern trap_s
.local trap_m
.local s_mode
.local init_bss
.local init_bss_done
.equ mtimecmp, 0x2004000
.equ mtime, 0x200bff8
.equ reg_size, 0x8
.align 3

_start:
	la sp,stack_top
	call main
