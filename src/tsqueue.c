#include "common.h"
#include "tsqueue.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/**
 * @brief Allocate the Queue container and the queue buffer to the heap.
 * @note Do not implement your calls to the heap here. Instead, please use
 *       the wrapper functions specified in common.h. These functions
 *       deal with common problems like exception handling and fragmentation.
 */
static ts_queue_t *ts_queue_alloc(const size_t cap)
{
  ts_queue_t *self = NULL;
  // TODO: Create a wrapper for this allocation call.
  self = (ts_queue_t *)calloc(1, sizeof(*self));
  // TODO: Create a wrapper for this allocation call.
  self->items = (int *)calloc(cap, sizeof(*self->items));
  return self;
}

/**
 * @brief Allocate a new Queue data structure to the heap. Do not allocate
 *        the queue properties here. Queue properties are allocated in
 *        queue_alloc() refer to it for more information. With that, this
 *        function sets up the Queue for usage.
 * @param cap The maximum capacity allow in the Queue data structure.
 */
ts_queue_t *ts_queue_new(const size_t cap)
{
  ts_queue_t *self = NULL;
  self = ts_queue_alloc(cap);

  atomic_init(&self->r, 0UL);
  atomic_init(&self->w, 0UL);

  self->cap = cap;
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
    __free((*self)->items);
    free(*self);
    *self = NULL;
  }
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
bool ts_queue_enqueue(ts_queue_t *self, const int item)
{
  if (self == NULL)
  {
    return false;
  }

  const uint64_t r = atomic_load(&self->r);
  const uint64_t w = atomic_fetch_add(&self->w, 1UL);

  if ((w - r) >= self->cap)
  {
    atomic_exchange(&self->w, w);
    return false;
  }

  self->items[w % self->cap] = item;

  return true;
}

/**
 * @brief Remove an item from the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return The item currently removed from the front of the Queue.
 */
int *ts_queue_dequeue(ts_queue_t *self)
{
  if (self == NULL)
  {
    return NULL;
  }

  const uint64_t w = atomic_load(&self->w);
  const uint64_t r = atomic_load(&self->r);

  if (r == w)
  {
    return NULL;
  }

  atomic_fetch_add(&self->r, 1UL);

  int *item = NULL;
  item = (int *)calloc(1, sizeof(*item));

  *item = self->items[r % self->cap];

  return item;
}

/**
 * @brief Return the item at the front of the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return A copy of the item currently at the front of the Queue.
 */
int *ts_queue_peek(ts_queue_t *self)
{
  if (self == NULL)
  {
    return NULL;
  }

  const uint64_t w = atomic_load(&self->w);
  const uint64_t r = atomic_load(&self->r);

  if (r == w)
  {
    return NULL;
  }

  int *item = NULL;
  item = (int *)calloc(1, sizeof(*item));

  *item = self->items[r % self->cap];

  return item;
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

  const uint64_t w = atomic_load(&self->w);
  const uint64_t r = atomic_load(&self->r);

  return (w - r);
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

  const uint64_t w = atomic_load(&self->w);
  const uint64_t r = atomic_load(&self->r);

  // Do not call the forward facing queue_size() method here.
  // That method adds redundant overhead to this method call.
  return 0UL == (w - r);
}
