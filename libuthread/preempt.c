#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

// set up sigaction
struct sigaction action;
sigset_t ss;
// set up initial time
struct itimerval timer;
// allow preemption
bool can_preempt = false;

void preempt_disable(void) { 
  if (can_preempt) {
    sigprocmask(SIG_BLOCK, &ss, NULL);
    printf("done!\n");
  }
}

void alarm_handler(int signum) {
  if(signum == SIGVTALRM){
    uthread_yield();
  }
  return;
}

void preempt_enable(void) {
  if (can_preempt) {
    sigprocmask(SIG_UNBLOCK, &ss, NULL); 
  }
}

void preempt_start(bool preempt) {
  if (!preempt) {
    return;
  }

  preempt = true;

  sigemptyset(&ss);
  sigaddset(&ss, SIGVTALRM);

  // set up the sig handler
  action.sa_handler = alarm_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGVTALRM, &action, NULL);

  // set up initial time
  timer.it_value.tv_sec = (1/HZ)*10e6;
  timer.it_value.tv_usec = 0;

  // set up incremented time
  timer.it_interval.tv_sec = (1/HZ)*10e6;
  timer.it_interval.tv_usec = 0;

  // set the timer
  int set = setitimer(ITIMER_VIRTUAL, &timer, NULL);
  if (set == -1) {
    return;
  }
}

void preempt_stop(void) {
  // if (can_preempt) {
    preempt_disable();
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
  // }
}