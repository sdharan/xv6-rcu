#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "rcu.h"

void rcu_read_lock()
{
    preempt_disable();
}

void rcu_read_unlock()
{
    preempt_enable();
}

void call_rcu()
{
    // Schedule this process to run in all the cpus.
    int i = 0;
    while (i < 2) {
        cprintf ("\nInside call rcu setting proc allowed cpu to %d", i);
        proc->allowed_cpu =  i++;
        yield();
    }
    proc->allowed_cpu = -1;
    // You are good to go.
    cprintf ("\n RCU wait done!");
}
