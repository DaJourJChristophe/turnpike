#include "common.h"
#include "queue.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Allocate the Queue container and the queue buffer to the heap.
 * @note Do not implement your calls to the heap here. Instead, please use
 *       the wrapper functions specified in common.h. These functions
 *       deal with common problems like exception handling and fragmentation.
 */
static queue_t *queue_alloc(const size_t cap)
{
  queue_t *self = NULL;
  self = (queue_t *)_calloc(1, sizeof(*self));
  self->data = (uint8_t *)_calloc(cap, sizeof(*self->data));
  return self;
}

/**
 * @brief Allocate a new Queue data structure to the heap. Do not allocate
 *        the queue properties here. Queue properties are allocated in
 *        queue_alloc() refer to it for more information. With that, this
 *        function sets up the Queue for usage.
 * @param cap The maximum capacity allow in the Queue data structure.
 */
queue_t *queue_new(const size_t cap, const size_t len)
{
  queue_t *self = NULL;
  self = queue_alloc(cap);
  self->cap = cap;
  self->len = len;
  return self;
}

/**
 * @brief Deallocate an existing Queue data structure from the heap.
 * @param self A double pointer to the Queue container.
 */
void __queue_destroy(queue_t **self)
{
  if (self != NULL && *self != NULL)
  {
    __free((*self)->data);
    ___free(*self);
    *self = NULL;
  }
}

static inline size_t always_inline __queue_unused(queue_t *self)
{
  if (true == self->b_inuse)
  {
    return self->a_start - self->b_end;
  }

  return self->cap - self->a_end;
}

static size_t queue_unused(queue_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "queue instance may not be null");
    exit(EXIT_FAILURE);
  }

  return __queue_unused(self);
}

static inline size_t always_inline __queue_used(queue_t *self)
{
  return (self->a_end - self->a_start) + self->b_end;
}

static size_t queue_used(queue_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "queue instance may not be null");
    exit(EXIT_FAILURE);
  }

  return __queue_used(self);
}

static inline void always_inline __queue_try_switch_to_b(queue_t *self)
{
  if ((self->cap - self->a_end) < (self->a_start - self->b_end))
  {
    self->b_inuse = true;
  }
}

static inline bool always_inline __queue_empty(queue_t *self)
{
  return self->a_start == self->a_end;
}

/**
 * @brief Determine of the Queue data structure is empty.
 * @param self A pointer to the Queue container.
 * @return Whether or not the Queue data structure is empty.
 */
bool queue_empty(queue_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "queue instance may not be null");
    exit(EXIT_FAILURE);
  }

  return __queue_empty(self);
}

/**
 * @brief Add an item to the Queue data structure. By default, the Queue
 *        adds items to the beginning of the data structure and removes
 *        items from the end of the data structure.
 *
 *        @note In the future, an argument will be added to this method
 *              so that end-users can change the queue scheme.
 *
 * @param self A pointer to the Queue container.
 * @param item The item to be added to the Queue data structure.
 * @return Whether or not the item was successfully added to the Queue.
 */
bool queue_enqueue(queue_t *self, const void *data)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "queue instance may not be null");
    exit(EXIT_FAILURE);
  }

  if (__queue_unused(self) < self->len)
  {
    return false;
  }

  if (true == self->b_inuse)
  {
    memcpy((self->data + self->b_end), data, self->len * sizeof(*self->data));
    self->b_end += self->len;
  }
  else
  {
    memcpy((self->data + self->a_end), data, self->len * sizeof(*self->data));
    self->a_end += self->len;
  }

  __queue_try_switch_to_b(self);
  return true;
}

/**
 * @brief Remove an item from the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return The item currently removed from the front of the Queue.
 */
void *queue_dequeue(queue_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "queue instance may not be null");
    exit(EXIT_FAILURE);
  }

  if (__queue_empty(self))
  {
    return NULL;
  }

  if (self->cap < (self->a_start + self->len))
  {
    return NULL;
  }

  void *data = NULL;
  data = _calloc(self->len, sizeof(*self->data));

  memcpy(data, (self->data + self->a_start), self->len * sizeof(*self->data));
  self->a_start += self->len;

  if (__queue_empty(self))
  {
    if (true == self->b_inuse)
    {
      self->a_start = 0;
      self->a_end   = self->b_end;
      self->b_end   = 0;
      self->b_inuse = false;
    }
    else
    {
      self->a_start = 0;
      self->a_end   = 0;
    }
  }

  __queue_try_switch_to_b(self);
  return data;
}

/**
 * @brief Return the item at the front of the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return A copy of the item currently at the front of the Queue.
 */
void *queue_peek(queue_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "queue instance may not be null");
    exit(EXIT_FAILURE);
  }

  if (self->cap < (self->a_start + self->len))
  {
    return NULL;
  }

  if (__queue_empty(self))
  {
    return NULL;
  }

  void *data = NULL;
  data = _calloc(self->len, sizeof(*self->data));

  memcpy(data, (self->data + self->a_start), self->len * sizeof(*self->data));

  return data;
}

/**
 * @brief Return the number of items currently in the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return The current size of the Queue data structure.
 */
size_t queue_size(queue_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "queue instance may not be null");
    exit(EXIT_FAILURE);
  }

  return __queue_used(self);
}
