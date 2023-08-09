#include "sched.h"
#include "put.h"
#include "rand.h"

#define offset (0xffffffe000000000 - 0x80000000)

#define Kernel_Page 0x80210000
#define LOW_MEMORY 0x80211000
#define PAGE_SIZE 4096UL
#define LAB_TEST_NUM 4
#include <stddef.h>

struct task_struct *current;
struct task_struct *task[NR_TASKS];
long PRIORITY_INIT_COUNTER[NR_TASKS] = {0, 1, 2, 3, 4};
long COUNTER_INIT_COUNTER[NR_TASKS] = {0, 1, 2, 3, 4};

int task_init_done = 0;
int task_test_done = 0;

extern void init_epc(void);
extern void __switch_to(struct task_struct *current, struct task_struct *next);

void task_init(void)
{
    puts("task init...\n");

    // initialize task[0]
    current = (struct task_struct *)Kernel_Page;
    current->state = TASK_RUNNING;
    current->counter = COUNTER_INIT_COUNTER[0];
    current->priority = PRIORITY_INIT_COUNTER[0];
    current->blocked = 0;
    current->pid = 0;
    task[0] = current;
    // task[0]->thread.sp = (unsigned long long)task[0] + TASK_SIZE;
    task[0]->thread.sp = (unsigned long long)task[0] + PAGE_SIZE;

    // set other 4 tasks
    struct task_struct* task_ptr = NULL;
    for (int i = 1; i <= LAB_TEST_NUM; ++i)
    {
        /* your code */
        // initialize task[i]
        task_ptr = (uint64_t)LOW_MEMORY + (uint64_t)PAGE_SIZE * (i - 1) + (uint64_t)offset;
        task_ptr->state = TASK_RUNNING;
        task_ptr->counter = COUNTER_INIT_COUNTER[i];
        task_ptr->priority = PRIORITY_INIT_COUNTER[i];
        task_ptr->blocked = 0;
        task_ptr->pid = i;
        task[i] = task_ptr;

        // task[i]->thread.ra = (unsigned long long)__init_sepc;
        task[i]->thread.ra = (unsigned long long)init_epc;

        // task[0]->thread.sp = (unsigned long long)task[0] + TASK_SIZE + PAGE_SIZE * i;
        task[i]->thread.sp = (unsigned long long)task[i] + PAGE_SIZE + offset;

        printf("[PID = %d] Process Create Successfully!\n", task[i]->pid);
    }
    task_init_done = 1;
}

void do_timer(void)
{
    if (!task_init_done)
        return;
    if (task_test_done)
        return;
    if (current == NULL)
        return;
    printf("[*PID = %d] Context Calculation: counter = %d,priority = %d\n",
           current->pid, current->counter, current->priority);
    // current process's counter -1, judge whether to schedule or go on.
    /* your code */

    current->counter--;
    if (current->counter <= 0)
    {
        schedule();
    }
}

// Select the next task to run. If all tasks are done(counter=0), set task0's
// counter to 1 and it would assign new test case.

void schedule(void)
{
    unsigned char next = 0;
    /* your code */
    long min_counter = -1;
    for (int i = NR_TASKS - 1; i > 0; i--)
    {
        if (task[i] == NULL)
        {
            continue;
        }
        else if (task[i]->state == TASK_RUNNING && task[i]->counter > 0)
        {
            if (min_counter == -1 || task[i]->counter < min_counter)
            {
                min_counter = task[i]->counter;
                next = i;
            }
        }
    }

    // no task remains counter
    if (next == 0)
    {
        // task[next]->counter = 1;
        for (int i = 0; i < 4; i++)
        {
            task[i]->counter = rand();
            task[i]->priority = rand();
        }
        schedule();
    }
    else if (current->pid != task[next]->pid)
    {
        printf(
            "[ %d -> %d ] Switch from task %d[%lx] to task %d[%lx], prio: %d, "
            "counter: %d\n",
            current->pid, task[next]->pid, current->pid,
            (unsigned long)current->thread.sp, task[next]->pid,
            (unsigned long)task[next], task[next]->priority, task[next]->counter);
    }
    switch_to(task[next]);
}

void switch_to(struct task_struct *next)
{
    struct task_struct *prev = NULL;
    if (next != current)
    {
        prev = current;
        current = next;
        __switch_to(prev, current);
    }
}

void dead_loop(void)
{
    while (1)
    {
        continue;
    }
}
