#ifndef TURNPIKE__THREAD_SAFE_QUEUE_H
#define TURNPIKE__THREAD_SAFE_QUEUE_H

#include <inttypes.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief A safe implementation of a Queue data structure. This data
 *        structure is based upon the Circular Buffer. It has a stateful
 *        capacity specification and read and write pointers. Every
 *        operation is a constant time operation.
 */
struct ts_queue
{
  uint8_t *data;
  size_t cap;
  size_t len;
  atomic_ulong a_start;
  atomic_ulong a_end;
  atomic_ulong b_end;
  atomic_bool b_inuse;
};

/**
 * @brief An alias for the Queue data struct.
 */
typedef struct ts_queue ts_queue_t;

/**
 * @brief Allocate a new Queue data structure to the heap.
 * @param cap The maximum capacity allow in the Queue data structure.
 */
ts_queue_t *ts_queue_new(const size_t cap, const size_t len);

/**
 * @brief Deallocate an existing Queue data structure from the heap.
 * @param self A double pointer to the Queue container.
 */
void __ts_queue_destroy(ts_queue_t **self);

/**
 * @brief Create a stack-pointer and pass it to queue_destroy() so that
 *        the queue pointer in the caller knows the queue no longer exists.
 * @param self A pointer to the Queue container.
 */
#define ts_queue_destroy(self) __ts_queue_destroy(&self)

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
bool ts_queue_enqueue(ts_queue_t *self, const void *data);

/**
 * @brief Remove an item from the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return The item currently removed from the front of the Queue.
 */
void *ts_queue_dequeue(ts_queue_t *self);

/**
 * @brief Return the item at the front of the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return A copy of the item currently at the front of the Queue.
 */
void *ts_queue_peek(ts_queue_t *self);

/**
 * @brief Return the number of items currently in the Queue data structure.
 * @param self A pointer to the Queue container.
 * @return The current size of the Queue data structure.
 */
size_t ts_queue_size(ts_queue_t *self);

/**
 * @brief Determine of the Queue data structure is empty.
 * @param self A pointer to the Queue container.
 * @return Whether or not the Queue data structure is empty.
 */
bool ts_queue_empty(ts_queue_t *self);

#endif/*TURNPIKE__THREAD_SAFE_QUEUE_H*/
