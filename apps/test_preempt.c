/*
 * Simple hello world test
 *
 * Tests the creation of a single thread and its successful return.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

bool quitT2;
void thread2(void *arg) {
  (void)arg;
  printf("!!!THREAD2!!!\n");
  quitT2 = true;
  printf("Bye\n");
  return;
}

void thread1(void *arg) {
  (void)arg;
  uthread_create(thread2, NULL);
  int counter = 0;
  printf("!!!THREAD1!!!\n");

  while (1) {
    counter++;
    if (quitT2) {
      break;
    }
  }
  printf("Bye\n");
  return;
}

int main(void) {
  uthread_run(false, thread1, NULL);
  return 0;
}