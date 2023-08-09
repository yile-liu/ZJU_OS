.section .text.entry

.extern test
.global trap_s
.extern handler_s
.equ reg_size, 0x8
.align 2

trap_s:
	# save caller-saved registers and sepc
	addi sp, sp, -136

	sd ra, 0(sp)
    sd t0, 8(sp)
    sd t1, 16(sp)
    sd t2, 24(sp)
    sd a0, 32(sp)
    sd a1, 40(sp)
    sd a2, 48(sp)
    sd a3, 56(sp)
    sd a4, 64(sp)
    sd a5, 72(sp)
    sd a6, 80(sp)
    sd a7, 88(sp)
    sd t3, 96(sp)
    sd t4, 104(sp)
    sd t5, 112(sp)
    sd t6, 120(sp)
	csrr t0, sepc
	sd t0, 128(sp)

	# call handler_s(scause)
	csrr a0, scause
	csrr a1, sepc
	call handler_s
	
	# load sepc and caller-saved registers
	ld ra, 0(sp)
    ld t0, 8(sp)
    ld t1, 16(sp)
    ld t2, 24(sp)
    ld a0, 32(sp)
    ld a1, 40(sp)
    ld a2, 48(sp)
    ld a3, 56(sp)
    ld a4, 64(sp)
    ld a5, 72(sp)
    ld a6, 80(sp)
    ld a7, 88(sp)
    ld t3, 96(sp)
    ld t4, 104(sp)
    ld t5, 112(sp)
    ld t6, 120(sp)
	ld t0, 128(sp)
	csrw sepc, t0
	
	addi sp, sp, 136
	
	sret


# /* 进程数据结构 */
# struct task_struct {
#   long state;     // 进程状态 Lab3中进程初始化时置为TASK_RUNNING
#   long counter;   // 运行剩余时间
#   long priority;  // 运行优先级 1最高 5最低
#   long blocked;
#   long pid;  // 进程标识符
#   //Above Size Cost: 40 bytes

#   struct thread_struct thread;  // 该进程状态段
# };
# struct thread_struct {
#   unsigned long long ra;
#   unsigned long long sp;
#   unsigned long long s0;
#   unsigned long long s1;
#   unsigned long long s2;
#   unsigned long long s3;
#   unsigned long long s4;
#   unsigned long long s5;
#   unsigned long long s6;
#   unsigned long long s7;
#   unsigned long long s8;
#   unsigned long long s9;
#   unsigned long long s10;
#   unsigned long long s11;
# };

.globl __switch_to
__switch_to:
	li    a4,  40 
	add   a3, a0, a4 # a0 prev
	add   a4, a1, a4 # a1 next
	# Save context into prev->thread
	# your code
	sd ra, 0(a3)
	sd sp, 8(a3)
	sd s0, 16(a3)
	sd s1, 24(a3)
	sd s2, 32(a3)
	sd s3, 40(a3)
	sd s4, 48(a3)
	sd s5, 56(a3)
	sd s6, 64(a3)
	sd s7, 72(a3)
	sd s8, 80(a3)
	sd s9, 88(a3)
	sd s10, 96(a3)
	sd s11, 104(a3)

	# Restore context from next->thread
	# your code
	ld ra, 0(a4)
	ld sp, 8(a4)
	ld s0, 16(a4)
	ld s1, 24(a4)
	ld s2, 32(a4)
	ld s3, 40(a4)
	ld s4, 48(a4)
	ld s5, 56(a4)
	ld s6, 64(a4)
	ld s7, 72(a4)
	ld s8, 80(a4)
	ld s9, 88(a4)
	ld s10, 96(a4)
	ld s11, 104(a4)

	# return to ra
	ret


.globl __init_sepc
__init_sepc:
	la  t0, test
	csrw sepc, t0
	sret
