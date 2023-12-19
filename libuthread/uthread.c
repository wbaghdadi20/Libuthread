#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "queue.h"
#include "uthread.h"

// global variables
struct uthread_tcb *current_thread = NULL;
struct uthread_tcb *prev_thread = NULL;
static struct uthread_tcb *IDLE;
queue_t ready_q;
queue_t zombie_q;

typedef enum ThreadState { 
  Ready, 
  Running, 
  Blocked, 
  Zombie 
} ThreadState;

typedef struct uthread_tcb {
  uthread_ctx_t context;
  void *stack;
  ThreadState state;
} uthread_tcb;

struct uthread_tcb *uthread_current(void) {
  /* TODO Phase 2/3 */
  return current_thread;
}

static void remove_zombies(queue_t q, void *data) {
  uthread_tcb *remove = (struct uthread_tcb*)data;
  uthread_tcb *del_tcb;
  uthread_ctx_destroy_stack(remove->stack);
  queue_dequeue(q, (void**)&del_tcb);
  free(del_tcb);
}

void switch_threads(void) {
  // get next availabe thread in ready queue
  int retval = queue_dequeue(ready_q, (void**)&current_thread);
  if (retval == -1) {
    uthread_ctx_switch(&(prev_thread->context), &(IDLE->context));
  }

  current_thread->state = Running;
  uthread_ctx_switch(&(prev_thread->context), &(current_thread->context));
  return;
}

void free_all(void) {
    // free queues
  queue_destroy(ready_q);
  queue_destroy(zombie_q);
  
  // free idle thread
  uthread_ctx_destroy_stack(IDLE->stack);
  free(IDLE);
  return;
}

void uthread_yield(void) {
  preempt_disable();
  // enqueue prev thread back in ready queue
  prev_thread = current_thread;
  prev_thread->state = Ready;
  queue_enqueue(ready_q, prev_thread);
  switch_threads();
  preempt_enable();
  return;
}

void uthread_exit(void) {
  preempt_disable();
  // enqueue zombie thread in zombie queue
  prev_thread = current_thread;
  prev_thread->state = Zombie;
  queue_enqueue(zombie_q, prev_thread);
  switch_threads();
  return;
}

int uthread_create(uthread_func_t func, void *arg) {
  uthread_tcb *new_thread = malloc(sizeof(uthread_tcb));

  // if failure to malloc thread
  if (!new_thread)
    return -1;

  new_thread->stack = uthread_ctx_alloc_stack();

  // if failure to allocate stack
  if (!new_thread->stack) {
    free(new_thread);
    return -1;
  }

  new_thread->state = Ready;

  // if failure to initialize thread
  int retval = uthread_ctx_init(&(new_thread->context), 
    new_thread->stack, func, arg);
  
  if (retval == -1) {
    free(new_thread->stack);
    free(new_thread);
    return -1;
  }

  queue_enqueue(ready_q, new_thread);

  return 0;
}

// passes in the first thread's function and arguments
int uthread_run(bool preempt, uthread_func_t func, void *arg) {
  ready_q = queue_create();
  zombie_q = queue_create();

  // create idle thread
  uthread_create(NULL, NULL);
  queue_dequeue(ready_q, (void**)&IDLE);
  current_thread = IDLE;
  current_thread->state = Running;
  prev_thread = current_thread;
  
  // create first user thread and check for failure
  if (uthread_create(func, arg) == -1) {
    uthread_ctx_destroy_stack(IDLE->stack);
    free(IDLE);
    queue_destroy(ready_q);
    queue_destroy(zombie_q);
    return -1;
  }

  // start preemption before running threads
  preempt_start(preempt);

  // at this point we have a thread in our queue and are able to switch contexts
  while (1) {

    // delete zombie threads
    queue_iterate(zombie_q, remove_zombies);

    if(queue_length(ready_q) == 0){
      free_all();
      preempt_stop();
      return 0;
    }
  
    // get next available thread in queue
    uthread_yield();
  }

  return -1;
}

void uthread_block(void) {
  // block previous thread
  preempt_disable();
  prev_thread = current_thread;
  prev_thread->state = Blocked;

  // get next available thread in ready queue
  switch_threads();
  preempt_enable();
  return;
}

void uthread_unblock(struct uthread_tcb *uthread) {
  uthread->state = Ready;
  queue_enqueue(ready_q, uthread);
  return;
}