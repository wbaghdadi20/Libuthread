# Project 2
***
# Queue API
```c
struct node {
  node *next;
  void *data;
};

struct queue {
  /* TODO Phase 1 */
  node *front;
  node *end;
  int len;
};
```

### Implementation
***
To design the queue, we decided to use a singly-linked list with 2 structs, one for pointing to the data, and another one to save the front and end nodes, as well as length of the queue.

```c
struct node {};
struct queue {};
```

### queue_enqueue()
***
The `queue_enqueue()` function pushes the data to the bottom of the queue by allocating memory for it. It checks if the queue is empty or not. If empty, the front and end of the queue points to it because there is only one node. Otherwise, it points to the previous last node's `next` attribute, and updates the end of the queue.

```c
int queue_enqueue(queue_t queue, void *data){}
```

### queue_dequeue()
***
The `queue_dequeue()` function pops the first item from the queue, and saves it in the address specified in the second parameter. The front of the queue now points to the next (second) item in the list.

```c
int queue_dequeue(queue_t queue, void **data)
```

### queue_delete()
***
The `queue_delete()` function first checks if the data the first node, or has to traverse the queue. If the data is at the top of the queue, it simply moves the queue head to the next node, and deletes the current one. If the data is in the middle of the queue, we save the head, and keep traversing until it finds the node, or until the current node doesn't point to another one.

```c
int queue_delete(queue_t queue, void *data)
```

### queue_iterate()
***
The `queue_iterate()` function saves the queue head in two pointers, one for traversing (current), another for reading data (previous). It simply loops until the current node is null, and executes the function using the previous node's data each iteration.

```c
int queue_iterate(queue_t queue, queue_func_t func){}
```

### Testing
***
We tested our queue implementation to make sure it was able to create, destroy, enqueue, dequeue, and iterate correctly. We also made sure it was able to handle multiple datatypes in `test_null()` in the tester file, and pushed integers, strings, and chars into the queue. Most of our testing was done with `queue_iterate()` and `queue_delete` to check for memory leaks. We also accounted for handling invalid values to check if the functions failed properly.

```c
int main(void) {
	test_create();
	test_queue_enqueue();
	test_queue_simple();
	test_queue_dequeue();
	test_queue_complex();
	test_null();
	test_delete_mid();
	test_delete_null();
	test_delete_start();
	test_delete_end();

	return 0;
}
```

# Uthread API
```c
queue_t ready_q;
queue_t zombie_q;

typedef enum ThreadState {} ThreadState;
typedef struct uthread_tcb {} uthread_tcb;
```

### Implementation
***
Our `uthread.c` file has a ready queue for all threads to be executed, a zombie queue for deleting completed threads, and an enum that specifies the thread state. 

### uthread_run()
***
The function `uthread_run()` creates the idle thread and initial thread, then infinitely loops until the queue is empty, then properly frees the thread stack pointer, and the thread control block. As it loops, it deletes all threads in `zombie_q` using `queue_iterate()`. To prevent memory leaks, we use the `dequeue()` function to grab the malloc'd TCB from the queue, and free the pointer to the top of the stack and the TCB itself. This function either returns when the queue is empty, or yields to the next ready thread in `ready_q`. We also have three global TCB structs to hold the current thread, previous thread, and idle thread.
```c
int uthread_run(bool preempt, uthread_func_t func, void *arg){}
```

### Thread Switching
***
Switching threads in `yield()`, `exit()`, and `block()` requires saving the current thread as the previous thread, dequeueing it from `ready_q`. We handle the edge case where there are no more threads to switch to, and the current thread simply switches to the idle thread. Since yielding to another context is a critical section, we disable preemption because we want yielding, exiting, or blocking to be atomic.
```c
void switch_threads(void){}
```

### Blocking and Unblocking
***
When a thread is blocked by a semaphore, it saves the previous thread, then it pops the next thread from the ready queue. Just like the `yield()` and `exit()` functions, it switches contexts to the next thread. When unblocked the thread is simply re-enqueued in `ready_q`.

```c
void uthread_block(void){}
void uthread_unblock(struct uthread_tcb *uthread){}
```

# Semaphore API
```c
struct semaphore {};
```

### Implementation
***
Our semaphore API utilizes the semaphore struct, which includes a queue for waiting threads, and counter for the number of threads in the queue. The semaphore allows the user to manage resources among threads. We use `waiting_q` to handle the edge case of threads grabbing resources when the count = 0, by allowing waiting threads to run later. 

### sem_down()
***
The `sem_down()` function simply decrements the counter after checking if anything is in `waiting_q`. If there is, then it will busy-wait, and block the current thread until there is at least one resource available. It also imposes an order such that waiting threads have the highest priority when grabbing resources. This is a critical section because we have to guarantee that a thread can properly grab a resource atomically.
```c
int sem_down(sem_t sem){}
```
### sem_up()
***
The `sem_up` function simply increments the counter after checking if any thread must be resumed from the `waiting_q`. Just like sem_down, it must be performed atomically, since the function directly affects the threads being able to free resources.
```c
int sem_up(sem_t sem){}
```
# Preemption
```c
struct sigaction action;
sigset_t ss;
struct itimerval timer;
bool can_preempt;
```
### Implementation
***
This code sets up a virtual timer using the `setitimer()` function. It specifies the initial and interval timer values in a struct itimerval passed as the second argument. The virtual timer generates a SIGVTALRM signal at a specified interval, and this signal is handled by the `alarm_handler()` function, which calls `uthread_yield()` to switch to another thread.

### preempt_start()
***
This function sets up a virtual timer for preemption and schedules an interrupt signal SIGVTALRM to fire periodically. When the signal is received, the `alarm_handler()` function is called, which executes `uthread_yield()` to switch to another thread.
```c
void preempt_start(bool preempt){}
```
### alarm_handler()
***
When the SIGVTALRM signal is raised, the operating system interrupts the current thread and switches control to the signal handler. The `alarm_handler()` function checks if the signal is SIGVTALRM, and if it is, it calls `uthread_yield()` to relinquish the CPU and allow another thread to execute. The purpose of this handler is to allow preemptive scheduling by interrupting a running thread periodically and allowing other threads to execute.
```c
void alarm_handler(int signum){}
```
### preempt_enable()
***
The preempt_enable() function enables preemption by unblocking the SIGVTALRM signal using `sigprocmask()`. The function checks the value of the can_preempt flag to ensure that preemption is actually enabled. If can_preempt is true, the signal mask for the current thread is modified to remove the SIGVTALRM signal from the set of blocked signals using `sigprocmask(SIG_UNBLOCK, &ss, NULL)`.
```c
void preempt_enable(void){}
```
### preempt_disable()
***
The preempt_disable() function disables preemption by blocking the SIGVTALRM signal using `sigprocmask()`. The function checks the value of the `can_preempt` flag to ensure that preemption is actually enabled. If `can_preempt` is true, the signal mask for the current thread is modified to add the SIGVTALRM signal to the set of blocked signals using `sigprocmask(SIG_BLOCK, &ss, NULL)`.
```c
void preempt_disable(void){}
```
### preempt_stop()
***
The preempt_stop function disables preemption and stops the virtual timer. It first calls `preempt_disable()` to disable preemption by blocking the SIGVTALRM signal with sigprocmask. It then calls setitimer with the ITIMER_VIRTUAL argument to stop the virtual timer by setting both the initial and interval timer values to zero.
```c
void preempt_stop(void){}
```
### Testing
***
The code tests the Uthread library by creating two threads. The first thread enters an infinite loop and repeatedly checks if the global variable quitT2 is true. Meanwhile, the second thread sets quitT2 to true after printing some output. When the first thread detects that quitT2 is true, it breaks out of the loop and also prints some output. This tests the preemption functionality of the Uthread library, which should allow the second thread to run and complete before returning to the first thread.