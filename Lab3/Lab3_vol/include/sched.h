#ifndef _SCHED_H
#define _SCHED_H

#define TASK_SIZE (4096)
#define THREAD_OFFSET (5 * 0x08)

#ifndef __ASSEMBLER__

/* task的最大数量 */
#define NR_TASKS 64

#define FIRST_TASK (task[0])
#define LAST_TASK (task[NR_TASKS - 1])

/* 定义task的状态，Lab3中task只需要一种状态。*/
#define TASK_RUNNING 0
// #define TASK_INTERRUPTIBLE       1
// #define TASK_UNINTERRUPTIBLE     2
// #define TASK_ZOMBIE              3
// #define TASK_STOPPED             4

#define PREEMPT_ENABLE 0
#define PREEMPT_DISABLE 1

/* Lab3中进程的数量以及每个进程初始的时间片 */
#define LAB_TEST_NUM 4
#define LAB_TEST_COUNTER 5

/* 当前进程 */
extern struct task_struct* current;

/* 进程指针数组 */
extern struct task_struct* task[NR_TASKS];

/* 进程状态段数据结构 */
struct thread_struct {
  unsigned long long ra;
  unsigned long long sp;
  unsigned long long s0;
  unsigned long long s1;
  unsigned long long s2;
  unsigned long long s3;
  unsigned long long s4;
  unsigned long long s5;
  unsigned long long s6;
  unsigned long long s7;
  unsigned long long s8;
  unsigned long long s9;
  unsigned long long s10;
  unsigned long long s11;
};

/* 进程数据结构 */
struct task_struct {
  long state;     // 进程状态 Lab3中进程初始化时置为TASK_RUNNING
  long counter;   // 运行剩余时间
  long priority;  // 运行优先级 1最高 5最低
  long blocked;
  long pid;  // 进程标识符
  // Above Size Cost: 40 bytes

  struct thread_struct thread;  // 该进程状态段
};

/* 进程初始化 创建四个dead_loop进程 */
void task_init(void);

/* 在时钟中断处理中被调用 */
void do_timer(void);

/* 调度程序 */
void schedule(void);

/* 切换当前任务current到下一个任务next */
void switch_to(struct task_struct* next);

extern void __switch_to(struct task_struct* prev, struct task_struct* next);

/* 死循环 */
void dead_loop(void);

#endif

#endif