#include "common.h"
#include "tsqueue.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Allocate the Queue container and the queue buffer to the heap.
 * @note Do not implement your calls to the heap here. Instead, please use
 *       the wrapper functions specified in common.h. These functions
 *       deal with common problems like exception handling and fragmentation.
 */
static ts_queue_t *ts_queue_alloc(const size_t cap)
{
  ts_queue_t *self = NULL;
  self = (ts_queue_t *)_calloc(1, sizeof(*self));
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
ts_queue_t *ts_queue_new(const size_t cap, const size_t len)
{
  ts_queue_t *self = NULL;
  self = ts_queue_alloc(cap);

  atomic_init(&self->a_start, 0UL);
  atomic_init(&self->a_end,   0UL);
  atomic_init(&self->b_end,   0UL);
  atomic_init(&self->b_inuse, false);

  self->cap = cap;
  self->len = len;

  return self;
}

/**
 * @brief Deallocate an existing Queue data structure from the heap.
 * @param self A double pointer to the Queue container.
 */
void __ts_queue_destroy(ts_queue_t **self)
{
  if (self != NULL && *self != NULL)
  {
    __free((*self)->data);
    ___free(*self);
    *self = NULL;
  }
}

static inline size_t always_inline __ts_queue_unused(ts_queue_t *self)
{
  if (true == atomic_load(&self->b_inuse))
  {
    return atomic_load(&self->a_start) - atomic_load(&self->b_end);
  }

  return self->cap - atomic_load(&self->a_end);
}

static size_t ts_queue_unused(ts_queue_t *self)
{
  if (self == NULL)
  {
    return 0;
  }

  return __ts_queue_unused(self);
}

static inline size_t always_inline __ts_queue_used(ts_queue_t *self)
{
  if (self == NULL)
  {
    return 0UL;
  }

  return (atomic_load(&self->a_end) - atomic_load(&self->a_start)) + atomic_load(&self->b_end);
}

static size_t ts_queue_used(ts_queue_t *self)
{
  if (self == NULL)
  {
    return 0UL;
  }

  return __ts_queue_used(self);
}

static inline void always_inline __ts_queue_try_switch_to_b(ts_queue_t *self)
{
  if ((self->cap - atomic_load(&self->a_end)) < (atomic_load(&self->a_start) - atomic_load(&self->b_end)))
  {
    atomic_store(&self->b_inuse, true);
  }
}

static inline bool always_inline __ts_queue_empty(ts_queue_t *self)
{
  return atomic_load(&self->a_start) == atomic_load(&self->a_end);
}

/**
 * @brief Determine of the Queue data structure is empty.
 * @param self A pointer to the Queue container.
 * @return Whether or not the Queue data structure is empty.
 */
bool ts_queue_empty(ts_queue_t *self)
{
  if (self == NULL)
  {
    return false;
  }

  return __ts_queue_empty(self);
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
bool ts_queue_enqueue(ts_queue_t *self, const void *data)
{
  if (self == NULL)
  {
    return false;
  }

  const bool     b_inuse = atomic_load(&self->b_inuse);
  const uint64_t a_start = atomic_load(&self->a_start);

  const uint64_t a_end   = atomic_fetch_add(&self->a_end, self->len);
  const uint64_t b_end   = atomic_fetch_add(&self->b_end, self->len);

  const size_t   unused  = (true == b_inuse) ? (a_start - b_end) : (self->cap - a_end);

  if ((self->cap - a_end) < (a_start - b_end))
  {
    atomic_store(&self->b_inuse, true);
  }

  if (unused < self->len)
  {
    atomic_exchange(&self->a_end,   a_end);
    atomic_exchange(&self->b_end,   b_end);
    atomic_exchange(&self->b_inuse, b_inuse);

    return false;
  }

  if (true == b_inuse)
  {
    atomic_exchange(&self->a_end, a_end);
  }
  else
  {
    atomic_exchange(&self->b_end, b_end);
  }

  memcpy((self->data + ((true == b_inuse) ? b_end : a_end)), data, self->len * sizeof(*self->data));

  return true;
}

/**
 * @brief Remove an item from the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return The item currently removed from the front of the Queue.
 */
void *ts_queue_dequeue(ts_queue_t *self)
{
  if (self == NULL)
  {
    return NULL;
  }

  const bool     b_inuse = atomic_load(&self->b_inuse);

  const uint64_t a_end   = atomic_load(&self->a_end);
  const uint64_t b_end   = atomic_load(&self->b_end);

  const uint64_t a_start = atomic_fetch_add(&self->a_start, self->len);

  if ((a_start == a_end) || (self->cap < (a_start + self->len)))
  {
    atomic_exchange(&self->a_start, a_start);
    return NULL;
  }

  void *data = NULL;
  data = _calloc(self->len, sizeof(*self->data));

  memcpy(data, (self->data + a_start), self->len * sizeof(*self->data));

  if (a_start == a_end)
  {
    if (true == b_inuse)
    {
      atomic_store(&self->a_start, 0UL);
      atomic_store(&self->a_end,   b_end);
      atomic_store(&self->b_end,   0UL);
      atomic_store(&self->b_inuse, false);
    }
    else
    {
      atomic_store(&self->a_start, 0UL);
      atomic_store(&self->a_end,   0UL);
    }
  }

  if ((self->cap - a_end) < (a_start - b_end))
  {
    atomic_store(&self->b_inuse, true);
  }

  return data;
}

/**
 * @brief Return the item at the front of the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return A copy of the item currently at the front of the Queue.
 */
void *ts_queue_peek(ts_queue_t *self)
{
  if (self == NULL)
  {
    return NULL;
  }

  if (self->cap < (atomic_load(&self->a_start) + self->len))
  {
    return NULL;
  }

  if (__ts_queue_empty(self))
  {
    return NULL;
  }

  void *data = NULL;
  data = _calloc(self->len, sizeof(*self->data));

  memcpy(data, (self->data + atomic_load(&self->a_start)), self->len * sizeof(*self->data));

  return data;
}

/**
 * @brief Return the number of items currently in the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return The current size of the Queue data structure.
 */
size_t ts_queue_size(ts_queue_t *self)
{
  if (self == NULL)
  {
    return 0UL;
  }

  return __ts_queue_used(self);
}
