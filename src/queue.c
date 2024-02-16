#include "common.h"
#include "queue.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

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
  self->items = (int *)_calloc(cap, sizeof(*self->items));
  return self;
}

/**
 * @brief Allocate a new Queue data structure to the heap. Do not allocate
 *        the queue properties here. Queue properties are allocated in
 *        queue_alloc() refer to it for more information. With that, this
 *        function sets up the Queue for usage.
 * @param cap The maximum capacity allow in the Queue data structure.
 */
queue_t *queue_new(const size_t cap)
{
  queue_t *self = NULL;
  self = queue_alloc(cap);
  self->cap = cap;
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

/**
 * @brief Remove an item from the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return The item currently removed from the front of the Queue.
 */
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

/**
 * @brief Return the item at the front of the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return A copy of the item currently at the front of the Queue.
 */
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

/**
 * @brief Return the number of items currently in the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return The current size of the Queue data structure.
 */
size_t queue_size(queue_t *self)
{
  if (self == NULL)
  {
    return 0UL;
  }

  return (self->w - self->r);
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
    return false;
  }

  // Do not call the forward facing queue_size() method here.
  // That method adds redundant overhead to this method call.
  return 0UL == (self->w - self->r);
}
