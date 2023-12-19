#include <stddef.h>
#include <stdlib.h>

#include "private.h"
#include "queue.h"
#include "sem.h"

typedef struct uthread_tcb *tcb_t;

// flag that prevents invalid reads from null queue
int null_q = 0;

struct semaphore {
  size_t count;
  queue_t waiting_q;
};

sem_t sem_create(size_t count) {
  sem_t init_sem = malloc(sizeof(struct semaphore));

  // if failure to malloc
  if (init_sem == NULL)
    return NULL;

  init_sem->count = count;
  init_sem->waiting_q = queue_create();

  // if waiting queue failed to create
  if (init_sem->waiting_q == NULL) {
    free(init_sem);
    return NULL;
  }

  return init_sem;
}

int sem_destroy(sem_t sem) {
  // if invalid sem
  if (sem == NULL || sem->waiting_q == NULL) {
    return -1;
  }

  // if threads are still being blocked on sem
  if (queue_length(sem->waiting_q) > 0) {
    return -1;
  }

  // destroy queue
  queue_destroy(sem->waiting_q);
  sem->waiting_q = NULL;
  null_q = 1;

  // destroy semaphore
  free(sem);
  sem = NULL;
  return 0;
}

int sem_down(sem_t sem) {
  // if invalid sem, do blocking if no resources available
  if (sem == NULL) {
    return -1;
  }

  preempt_disable();

  // keep busy waiting until one resource is free
  while (sem->count == 0) {
    null_q = 0;

    // grab current queue, enqueue it
    queue_enqueue(sem->waiting_q, uthread_current());
    uthread_block();
  }

  // check if waiting_q is null
  if (null_q)
    return -1;

  sem->count--;

  preempt_enable();
  return 0;
}

int sem_up(sem_t sem) {
  // if sem is invalid
  if (sem == NULL) {
    return -1;
  }

  preempt_disable();

  /* Wake up first in line */
  if (queue_length(sem->waiting_q) > 0) {
    tcb_t ready_thread;
    queue_dequeue(sem->waiting_q, (void **)&ready_thread);
    uthread_unblock(ready_thread);
  }

  // increment by 1, wake up any waiting threads
  sem->count += 1;

  preempt_enable();

  return 0;
}
