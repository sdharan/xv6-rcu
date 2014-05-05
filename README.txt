RCU:

I have implemented a rudimentary form of RCU for the xv6 kernel. Basically it works through a set of modifications for the cpu structure and the process structures.

Changes to Scheduling Mechanisms:

Each process can set (optionally) a processor affinity flag in proc->allowed_cpu. If the value is set to a number which is not -1, then the process is picked and processed only by the processor whos cpu->uid matches that of the proc->allowed_cpu. 

Additionally, each cpu also maintains an internal flag cpu->prempt_disable_count which when set to a number greater than zero, will not allow the scheduler to preempt the current running process.

Algorithm:

With the above two changes, we can enable a very basic version of RCU, albeit a version where the writer may wait for a period that is greater than that is strictly required. Every time a process enters the critical section and reads a shared object, it notifies the cpu to not preempt it by calling the system call rcureadlock(), which internally sets the cpu preempt_disable_count to greater than zero. Every time the process leaves the read critical section, it unlocks the critical section by calling rcureadunlock() system call, which enables preemption of the process. 

When a writer wants to update the critical section, the writer process has to call rcuwait() which will wait for a period which ensures no readers are reading the shared object and returns. The writer can update the object as soon as rcuwait returns. rcuwait() system call internally works by setting the processor affinity incrementally to all processors from 0 to NCPU. This means that the process has to execute in all the processors before returning. Since any reader in the critial section would typically disable preemption in the critical section, this mechanism ensures that all readers are outside the critical section. The rcu wait system call simply returns as soon as it executes once in all the processors. 

The issue with this approach is that the writer may have to wait for a period which is may be longer than that which is strictly required as xv6 follows a fair round robin scheduling. Since setting processor affinity is done once every round, it may have to wait for upto NCPU rounds before returinng to the writer. 

Testing:

In order to test this, i wrote a uprog called testrcu which basically does this

1) create a shared memory region and set all values in page to 'A';
2) In child, read the memory for a long period in critical section after enabling it through rcureadlock()
3) In parent, call rcuwait() to wait for the readers to finish before updating memory to 'B's


If there is no rcuwait() in the parent,, then the child is written to throw errors as soon as it reads a value which is not a 'A'.



