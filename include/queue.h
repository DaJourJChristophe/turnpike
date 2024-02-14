#ifndef TURNPIKE__QUEUE_H
#define TURNPIKE__QUEUE_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

struct queue
{
  int *items;
  size_t cap;
  uint64_t r;
  uint64_t w;
};

typedef struct queue queue_t;

queue_t *queue_new(const size_t cap);

void __queue_destroy(queue_t **self);

#define queue_destroy(self) __queue_destroy(&self)

bool queue_enqueue(queue_t *self, const int item);

int *queue_dequeue(queue_t *self);

int *queue_peek(queue_t *self);

size_t queue_size(queue_t *self);

bool queue_empty(queue_t *self);

#endif/*TURNPIKE__QUEUE_H*/
