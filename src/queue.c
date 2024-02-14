#include "common.h"
#include "queue.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

static queue_t *queue_alloc(const size_t cap)
{
  queue_t *self = NULL;
  self = (queue_t *)calloc(1, sizeof(*self));
  self->items = (int *)calloc(cap, sizeof(*self->items));
  return self;
}

queue_t *queue_new(const size_t cap)
{
  queue_t *self = NULL;
  self = queue_alloc(cap);
  self->cap = cap;
  return self;
}

void __queue_destroy(queue_t **self)
{
  if (self != NULL && *self != NULL)
  {
    __free((*self)->items);
    free(*self);
    *self = NULL;
  }
}

bool queue_enqueue(queue_t *self, const int item)
{
  if (self == NULL)
  {
    return false;
  }

  if ((self->w - self->r) >= self->cap)
  {
    return false;
  }

  self->items[self->w++ % self->cap] = item;
  return true;
}

int *queue_dequeue(queue_t *self)
{
  if (self == NULL)
  {
    return NULL;
  }

  if (self->r == self->w)
  {
    return NULL;
  }

  int *item = NULL;
  item = (int *)calloc(1, sizeof(*item));
  *item = self->items[self->r++ % self->cap];
  return item;
}

int *queue_peek(queue_t *self)
{
  if (self == NULL)
  {
    return NULL;
  }

  if (self->r == self->w)
  {
    return NULL;
  }

  int *item = NULL;
  item = (int *)calloc(1, sizeof(*item));
  *item = self->items[self->r % self->cap];
  return item;
}

size_t queue_size(queue_t *self)
{
  if (self == NULL)
  {
    return 0UL;
  }

  return (self->w - self->r);
}

bool queue_empty(queue_t *self)
{
  if (self == NULL)
  {
    return false;
  }

  return 0UL == (self->w - self->r);
}
