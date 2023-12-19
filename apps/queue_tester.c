#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)                                                    \
  do {                                                                         \
    printf("ASSERT: " #assert " ... ");                                        \
    if (assert) {                                                              \
      printf("PASS\n");                                                        \
    } else {                                                                   \
      printf("FAIL\n");                                                        \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

/* Create */
void test_create(void) {
  fprintf(stderr, "*** TEST create ***\n");

  TEST_ASSERT(queue_create() != NULL);
}

static void iterator_inc(queue_t q, void *data) {
  int *a = (int *)data;

  if (*a == 42)
    queue_delete(q, data);
  else
    *a += 1;
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void) {
  int data = 3, *ptr, *ptr1, *ptr2;
  int data2 = 5;
  int data3 = 10;
  queue_t q;

  fprintf(stderr, "\n*** TEST queue_simple ***\n");

  q = queue_create();
  queue_enqueue(q, &data);
  queue_enqueue(q, &data2);
  queue_enqueue(q, &data3);

  // pop the nodes from the queue
  queue_dequeue(q, (void **)&ptr);
  printf("ptr: %d\n", *ptr);

  queue_dequeue(q, (void **)&ptr1);
  printf("ptr1: %d\n", *ptr1);

  queue_dequeue(q, (void **)&ptr2);
  printf("ptr2: %d\n", *ptr2);

  free(q);

  // check if the value from the deqeue matches with data
  TEST_ASSERT(ptr == &data);
  TEST_ASSERT(ptr1 == &data2);
  TEST_ASSERT(ptr2 == &data3);
}

void test_queue_complex(void) {
  int data = 3, *ptr, *ptr1, *ptr2;
  int data2 = 5;
  int data_new = 42;
  int data3 = 10;
  queue_t q;

  fprintf(stderr, "\n*** TEST queue_complex ***\n");

  q = queue_create();
  queue_enqueue(q, &data);
  queue_enqueue(q, &data_new);
  queue_enqueue(q, &data2);
  queue_enqueue(q, &data3);
  int iterated = queue_iterate(q, iterator_inc);

  // pop nodes
  queue_dequeue(q, (void **)&ptr);
  printf("ptr: %d\n", *ptr);

  queue_dequeue(q, (void **)&ptr1);
  printf("ptr1: %d\n", *ptr1);

  queue_dequeue(q, (void **)&ptr2);
  printf("ptr2: %d\n", *ptr2);

  queue_destroy(q);

  // check if the value from the deqeue matches with data
  TEST_ASSERT(iterated == 0);
  TEST_ASSERT(ptr == &data);
}

void test_null(void) {
  int data = 3, *ptr, *ptr1, *ptr2;
  char *data2 = "hey";
  char data3 = 'a';
  queue_t q = (struct queue *)NULL;

  fprintf(stderr, "\n*** TEST null ***\n");

  // test enqueue on null
  int data_res = queue_enqueue(q, &data);
  int data2_res = queue_enqueue(q, &data2);
  int data3_res = queue_enqueue(q, &data3);
  int iterated = queue_iterate(q, iterator_inc);

  // test destroy on null
  queue_dequeue(q, (void **)&ptr);
  queue_dequeue(q, (void **)&ptr1);
  queue_dequeue(q, (void **)&ptr2);
  queue_destroy(q);

  // make sure it returns -1
  TEST_ASSERT(iterated == -1);
  TEST_ASSERT(data_res == -1);
  TEST_ASSERT(data2_res == -1);
  TEST_ASSERT(data3_res == -1);
}

void test_delete_mid(void) {
  int data = 3, *ptr, *ptr2;
  int data2 = 5;
  int data3 = 10;
  queue_t q;

  fprintf(stderr, "\n*** TEST delete_end ***\n");

  q = queue_create();
  queue_enqueue(q, &data);
  queue_enqueue(q, &data2);
  queue_enqueue(q, &data3);
  int iterated = queue_iterate(q, iterator_inc);
  int og_value = queue_length(q);

  // replace this with queue_destroy() later
  int ret_delete = queue_delete(q, &data2);

  printf("length: %d\n", queue_length(q));

  // dequeue everything
  queue_dequeue(q, (void **)&ptr);
  printf("ptr: %d\n", *ptr);

  queue_dequeue(q, (void **)&ptr2);
  printf("ptr1: %d\n", *ptr2);

  queue_destroy(q);

  TEST_ASSERT(ret_delete == 0);
  TEST_ASSERT(og_value == 3);
  TEST_ASSERT(iterated == -1);
}

void test_queue_enqueue(void) {

  char c = 'l', *ptr;
  int a = 5, *ptr2;
  char *string = "hello world", *ptr3;

  queue_t queue;
  queue = queue_create();

  int c_result = queue_enqueue(queue, &c);
  int a_result = queue_enqueue(queue, &a);
  int str_result = queue_enqueue(queue, &string);

  int dequeue_c = queue_dequeue(queue, (void **)&ptr);
  int dequeue_a = queue_dequeue(queue, (void **)&ptr2);
  int dequeue_str = queue_dequeue(queue, (void **)&ptr3);

  queue_destroy(queue);

  TEST_ASSERT(c_result == 0);
  TEST_ASSERT(a_result == 0);
  TEST_ASSERT(str_result == 0);
  TEST_ASSERT(dequeue_c == 0);
  TEST_ASSERT(dequeue_a == 0);
  TEST_ASSERT(dequeue_str == 0);
}

void test_queue_dequeue(void) {

  int ptr;
  queue_t queue = queue_create();
  int dq = queue_dequeue(NULL, (void **)&ptr);

  queue_destroy(queue);

  TEST_ASSERT(dq == -1);
}

void test_delete_start(void) {

  char c = 'l';
  int a = 5, *ptr2;
  char *string = "hello world", *ptr3;

  queue_t queue;
  queue = queue_create();

  int c_result = queue_enqueue(queue, &c);
  int a_result = queue_enqueue(queue, &a);
  int str_result = queue_enqueue(queue, &string);

  int dequeue_c = queue_delete(queue, &c);
  int dequeue_a = queue_dequeue(queue, (void **)&ptr2);
  int dequeue_str = queue_dequeue(queue, (void **)&ptr3);

  queue_destroy(queue);

  TEST_ASSERT(c_result == 0);
  TEST_ASSERT(a_result == 0);
  TEST_ASSERT(str_result == 0);
  TEST_ASSERT(dequeue_c == 0);
  TEST_ASSERT(dequeue_a == 0);
  TEST_ASSERT(dequeue_str == 0);
}

void test_delete_end(void) {

  char c = 'l', *ptr;
  int a = 5, *ptr2;
  char *string = "hello world";

  queue_t queue;
  queue = queue_create();

  int c_result = queue_enqueue(queue, &c);
  int a_result = queue_enqueue(queue, &a);
  int str_result = queue_enqueue(queue, &string);

  int delete_str = queue_delete(queue, &string);
  int dequeue_c = queue_dequeue(queue, (void **)&ptr);
  int dequeue_a = queue_dequeue(queue, (void **)&ptr2);

  queue_destroy(queue);

  TEST_ASSERT(c_result == 0);
  TEST_ASSERT(a_result == 0);
  TEST_ASSERT(str_result == 0);
  TEST_ASSERT(dequeue_c == 0);
  TEST_ASSERT(dequeue_a == 0);
  TEST_ASSERT(delete_str == 0);
}

void test_delete_null(void) {

  char c = 'l', *ptr;
  int a = 5, *ptr2;
  int b = 10;

  queue_t queue;
  queue = queue_create();

  int c_result = queue_enqueue(queue, &c);
  int a_result = queue_enqueue(queue, &a);
  int str_result = queue_enqueue(queue, &b);

  printf("DELETE!\n");
  int delete_str = queue_delete(queue, &b);
  TEST_ASSERT(delete_str == 0);
  delete_str = queue_delete(queue, &b);
  int dequeue_c = queue_dequeue(queue, (void **)&ptr);
  int dequeue_a = queue_dequeue(queue, (void **)&ptr2);

  queue_destroy(queue);

  TEST_ASSERT(c_result == 0);
  TEST_ASSERT(a_result == 0);
  TEST_ASSERT(str_result == 0);
  TEST_ASSERT(dequeue_c == 0);
  TEST_ASSERT(dequeue_a == 0);
  TEST_ASSERT(delete_str == -1);
}

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