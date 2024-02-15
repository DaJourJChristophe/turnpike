#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include <cmocka/cmocka.h>

#include "queue.h"

#include <pthread.h>
#include <stdio.h>
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

queue_t *target = NULL;

void *proca(void *arg)
{
  int i;

  for (i = 0; i < 5000000; i++)
  {
    if (false == queue_enqueue(target, 1))
    {
      fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue value");
      exit(EXIT_FAILURE);
    }
  }

  return NULL;
}

static int suma = 0;
static int sumb = 0;

void *procb(void *arg)
{
  int *item = NULL;

  while (NULL != (item = queue_dequeue(target)))
  {
    suma += *item;
  }

  return NULL;
}

void *procc(void *arg)
{
  int *item = NULL;

  while (NULL != (item = queue_dequeue(target)))
  {
    sumb += *item;
  }

  return NULL;
}

static void queue_thread_safety_test(void unused **state)
{
  const size_t cap = 10000000;

  pthread_t t1;
  pthread_t t2;
  pthread_t t3;
  pthread_t t4;

  target = queue_new(cap);

  assert_true(pthread_create(&t1, NULL, &proca, NULL) >= 0);
  assert_true(pthread_create(&t2, NULL, &proca, NULL) >= 0);

  assert_true(pthread_join(t1, NULL) >= 0);
  assert_true(pthread_join(t2, NULL) >= 0);

  assert_true(pthread_create(&t3, NULL, &procb, NULL) >= 0);

  // Do not attempt to run the second thread. Just believe it results
  // in undefined behavior, most likely due to the race condition.
  // pthread_create(&t4, NULL, &procc, NULL);

  assert_true(pthread_join(t3, NULL) >= 0);

  // The second thread was never run so do not bother waiting for it.
  // pthread_join(t4, NULL);

  queue_destroy(target);

  assert_int_not_equal((suma + sumb), cap);
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
    cmocka_unit_test(queue_thread_safety_test),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
