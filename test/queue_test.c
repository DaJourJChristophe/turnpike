#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include <cmocka/cmocka.h>

#include "queue.h"

#include <stdlib.h>

#ifndef unused
#define unused __attribute__ ((unused))
#endif/*unused*/

static void queue_new_test(void unused **state)
{
  const size_t cap = 10;
  queue_t *queue = NULL;

  queue = queue_new(cap);
  assert_non_null(queue);

  queue_destroy(queue);
  assert_null(queue);
}

static void queue_enqueue_test(void unused **state)
{
  const size_t cap = 10;
  queue_t *queue = NULL;

  queue = queue_new(cap);
  // assert_non_null(queue);

  assert_true(queue_enqueue(queue, 1));
  assert_int_equal(queue->items[0], 1);

  queue_destroy(queue);
  // assert_null(queue);
}

static void queue_dequeue_test(void unused **state)
{
  const size_t cap = 10;
  queue_t *queue = NULL;

  queue = queue_new(cap);
  assert_non_null(queue);

  assert_true(queue_enqueue(queue, 1));
  const int *item = queue_dequeue(queue);
  assert_non_null(item);
  assert_int_equal(*item, 1);
  free((void *)item);
  item = NULL;

  queue_destroy(queue);
  assert_null(queue);
}

static void queue_peek_test(void unused **state)
{
  const size_t cap = 10;
  queue_t *queue = NULL;

  queue = queue_new(cap);
  assert_non_null(queue);

  assert_true(queue_enqueue(queue, 1));
  const int *item = queue_peek(queue);
  assert_non_null(item);
  assert_int_equal(*item, 1);
  free((void *)item);
  item = NULL;

  queue_destroy(queue);
  assert_null(queue);
}

static void queue_size_test(void unused **state)
{
  const size_t cap = 10;
  queue_t *queue = NULL;

  queue = queue_new(cap);
  assert_non_null(queue);

  assert_true(queue_enqueue(queue, 1));
  assert_int_equal(queue_size(queue), 1);

  queue_destroy(queue);
  assert_null(queue);
}

static void queue_empty_test(void unused **state)
{
  const size_t cap = 10;
  queue_t *queue = NULL;

  queue = queue_new(cap);
  assert_non_null(queue);

  assert_true(queue_empty(queue));

  assert_true(queue_enqueue(queue, 1));
  assert_false(queue_empty(queue));

  queue_destroy(queue);
  assert_null(queue);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(queue_new_test),
    cmocka_unit_test(queue_enqueue_test),
    cmocka_unit_test(queue_dequeue_test),
    cmocka_unit_test(queue_peek_test),
    cmocka_unit_test(queue_size_test),
    cmocka_unit_test(queue_empty_test),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
