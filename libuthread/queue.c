#include "queue.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct node node;

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

queue_t queue_create(void) {
  queue_t new_queue = malloc(sizeof(struct queue));

  // if failure to malloc, return null
  if(new_queue == NULL) return NULL;

  new_queue->front = NULL;
  new_queue->end = NULL;
  new_queue->len = 0;
  return new_queue;
}

int queue_destroy(queue_t queue) {
  // check if not ready to destroy
  if (queue == NULL || queue->len > 0) {
    return -1;
  }

  free(queue);
  return 0;
}

int queue_enqueue(queue_t queue, void *data) {
  // if invalid queue or no data, return
  if (queue == NULL || data == NULL) {
    return -1;
  }

  // create temp node
  node *add = malloc(sizeof(node));
  if(!add) return -1;

  // save data in the node
  add->data = data;
  add->next = NULL;
  queue->len++;

  // add node to the queue
  if (queue->len > 1) {
    queue->end->next = add;
    queue->end = add;
  } else {
    // do this if empty
    queue->end = add;
    queue->front = add;
  }

  return 0;
}

int queue_dequeue(queue_t queue, void **data) {
  // if invalid queue or no data, return
  if (queue == NULL) {
    return -1;
  } else if (!queue->len || data == NULL){
    return -1;
  }

  // temporarily hold top of stack
  *data = queue->front->data;
  node *remove = queue->front;

  // point to the next-oldest node
  queue->front = queue->front->next;
  queue->len--;
  free(remove);

  return 0;
}

int queue_delete(queue_t queue, void *data) {
  /* TODO Phase 1 */
  //if invalid queue or no data, return
  if (queue == NULL) {
    return -1;
  } else if (!queue->len || data == NULL) {
    return -1;
  }

  // if the data is in the first node, dequeue
  if (queue->front->data == data) {
    node *tmp = queue->front;
    queue->front = queue->front->next;
    queue->len--;
    free(tmp);
    return 0;
  }

  // look for the data in the queue
  node *current = queue->front;
  while (current->next != NULL) {
    // data is found
    if (current->next->data == data) {
      node *tmp = current->next;
      queue->len--;

      // check if it's the last node
      if (queue->end->data == data) {
        // point end to prev node
        queue->end = current;
        queue->end->next = NULL;
      } else {
        // check if its in the middle
        current->next = tmp->next;
      }

      free(tmp);
      return 0;
    }

    // traverse
    current = current->next;
  }

  return -1;
}

int queue_iterate(queue_t queue, queue_func_t func) {
  /* TODO Phase 1 */
  // if no valid queue or function, return
  if (queue == NULL || func == NULL) {
    return -1;
  }

  node *curr = queue->front;
  node *prev = queue->front;

  while (curr != NULL) {
    // look ahead to next node
    curr = curr->next;
    func(queue, prev->data);
    prev = curr;
  }

  return 0;
}

int queue_length(queue_t queue) { return queue->len; }