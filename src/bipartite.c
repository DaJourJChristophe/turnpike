#include "bipartite.h"
#include "common.h"

#include <pthread.h>
#include <stdatomic.h>
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
static bipartite_queue_t *bipartite_queue_alloc(const size_t cap)
{
  bipartite_queue_t *self = NULL;
  self = (bipartite_queue_t *)_calloc(1, sizeof(*self));
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
bipartite_queue_t *bipartite_queue_new(const size_t cap, const size_t len)
{
  bipartite_queue_t *self = NULL;
  self = bipartite_queue_alloc(cap);

  if (pthread_mutex_init(&self->lock, NULL) < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not init mutex lock");
    exit(EXIT_FAILURE);
  }

  atomic_init(&self->r, 0UL);
  atomic_init(&self->w, 0UL);

  self->cap = cap;
  self->len = len;

  return self;
}

/**
 * @brief Deallocate an existing Queue data structure from the heap.
 * @param self A double pointer to the Queue container.
 */
void __bipartite_queue_destroy(bipartite_queue_t **self)
{
  if (self != NULL && *self != NULL)
  {
    __free((*self)->data);
    ___free(*self);
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
bool bipartite_queue_enqueue(bipartite_queue_t *self, const void *data)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "queue instance may not be null");
    exit(EXIT_FAILURE);
  }

  if (pthread_mutex_lock(&self->lock) < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not lock mutex");
    exit(EXIT_FAILURE);
  }

  const uint64_t r = atomic_load(&self->r);
  const uint64_t w = atomic_fetch_add(&self->w, self->len);

  if ((w - r) >= self->cap)
  {
    atomic_exchange(&self->w, w);
    return false;
  }

  memcpy((self->data + (w % self->cap)), data, self->len * sizeof(*self->data));

  if (pthread_mutex_unlock(&self->lock) < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not unlock mutex");
    exit(EXIT_FAILURE);
  }

  return true;
}

/**
 * @brief Remove an item from the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return The item currently removed from the front of the Queue.
 */
void *bipartite_queue_dequeue(bipartite_queue_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "queue instance may not be null");
    exit(EXIT_FAILURE);
  }

  if (pthread_mutex_lock(&self->lock) < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not lock mutex");
    exit(EXIT_FAILURE);
  }

  const uint64_t w = atomic_load(&self->w);
  const uint64_t r = atomic_fetch_add(&self->r, self->len);

  if (r == w)
  {
    atomic_exchange(&self->r, r);
    return NULL;
  }

  void *item = NULL;
  item = calloc(self->len, sizeof(*item));

  memcpy(item, (self->data + (r % self->cap)), self->len * sizeof(*self->data));

  if (pthread_mutex_unlock(&self->lock) < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not unlock mutex");
    exit(EXIT_FAILURE);
  }

  return item;
}

/**
 * @brief Return the item at the front of the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return A copy of the item currently at the front of the Queue.
 */
void *bipartite_queue_peek(bipartite_queue_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "queue instance may not be null");
    exit(EXIT_FAILURE);
  }

  if (pthread_mutex_lock(&self->lock) < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not lock mutex");
    exit(EXIT_FAILURE);
  }

  const uint64_t w = atomic_load(&self->w);
  const uint64_t r = atomic_load(&self->r);

  if (r == w)
  {
    return NULL;
  }

  void *item = NULL;
  item = calloc(self->len, sizeof(*item));

  memcpy(item, (self->data + (r % self->cap)), self->len * sizeof(*self->data));

  if (pthread_mutex_unlock(&self->lock) < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not unlock mutex");
    exit(EXIT_FAILURE);
  }

  return item;
}

/**
 * @brief Return the number of items currently in the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return The current size of the Queue data structure.
 */
size_t bipartite_queue_size(bipartite_queue_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "queue instance may not be null");
    exit(EXIT_FAILURE);
  }

  if (pthread_mutex_lock(&self->lock) < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not lock mutex");
    exit(EXIT_FAILURE);
  }

  const uint64_t w = atomic_load(&self->w);
  const uint64_t r = atomic_load(&self->r);

  if (pthread_mutex_unlock(&self->lock) < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not unlock mutex");
    exit(EXIT_FAILURE);
  }

  return (w - r);
}

/**
 * @brief Determine of the Queue data structure is empty.
 * @param self A pointer to the Queue container.
 * @return Whether or not the Queue data structure is empty.
 */
bool bipartite_queue_empty(bipartite_queue_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "queue instance may not be null");
    exit(EXIT_FAILURE);
  }

  if (pthread_mutex_lock(&self->lock) < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not lock mutex");
    exit(EXIT_FAILURE);
  }

  const uint64_t w = atomic_load(&self->w);
  const uint64_t r = atomic_load(&self->r);

  if (pthread_mutex_unlock(&self->lock) < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not unlock mutex");
    exit(EXIT_FAILURE);
  }

  // Do not call the forward facing queue_size() method here.
  // That method adds redundant overhead to this method call.
  return 0UL == (w - r);
}
