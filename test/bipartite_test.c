#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include <cmocka/cmocka.h>

#include "bipartite.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef unused
#define unused __attribute__ ((unused))
#endif/*unused*/

static void bipartite_queue_new_test(void unused **state)
{
  const size_t cap = 10;
  bipartite_queue_t *queue = NULL;

  queue = bipartite_queue_new(cap, sizeof(int));
  assert_non_null(queue);

  bipartite_queue_destroy(queue);
  assert_null(queue);
}

static void bipartite_queue_enqueue_test(void unused **state)
{
  const size_t cap = 10;
  bipartite_queue_t *queue = NULL;

  queue = bipartite_queue_new(cap, sizeof(int));
  // assert_non_null(queue);

  assert_true(bipartite_queue_enqueue(queue, &(int){1}));
  int x = 0;
  memcpy(&x, queue->data, sizeof(int));
  assert_int_equal(x, 1);

  bipartite_queue_destroy(queue);
  // assert_null(queue);
}

static void bipartite_queue_dequeue_test(void unused **state)
{
  const size_t cap = 10;
  bipartite_queue_t *queue = NULL;

  queue = bipartite_queue_new(cap, sizeof(int));
  assert_non_null(queue);

  assert_true(bipartite_queue_enqueue(queue, &(int){1}));
  const int *item = bipartite_queue_dequeue(queue);
  assert_non_null(item);
  assert_int_equal(*item, 1);
  free((void *)item);
  item = NULL;

  bipartite_queue_destroy(queue);
  assert_null(queue);
}

static void bipartite_queue_peek_test(void unused **state)
{
  const size_t cap = 10;
  bipartite_queue_t *queue = NULL;

  queue = bipartite_queue_new(cap, sizeof(int));
  assert_non_null(queue);

  assert_true(bipartite_queue_enqueue(queue, &(int){1}));
  const int *item = bipartite_queue_peek(queue);
  assert_non_null(item);
  assert_int_equal(*item, 1);
  free((void *)item);
  item = NULL;

  bipartite_queue_destroy(queue);
  assert_null(queue);
}

static void bipartite_queue_size_test(void unused **state)
{
  const size_t cap = 10;
  bipartite_queue_t *queue = NULL;

  queue = bipartite_queue_new(cap, sizeof(int));
  assert_non_null(queue);

  assert_true(bipartite_queue_enqueue(queue, &(int){1}));
  assert_int_equal(bipartite_queue_size(queue), sizeof(int));

  bipartite_queue_destroy(queue);
  assert_null(queue);
}

static void bipartite_queue_empty_test(void unused **state)
{
  const size_t cap = 10;
  bipartite_queue_t *queue = NULL;

  queue = bipartite_queue_new(cap, sizeof(int));
  assert_non_null(queue);

  assert_true(bipartite_queue_empty(queue));

  assert_true(bipartite_queue_enqueue(queue, &(int){1}));
  assert_false(bipartite_queue_empty(queue));

  bipartite_queue_destroy(queue);
  assert_null(queue);
}

static bipartite_queue_t *target = NULL;

static void *proca(void *arg)
{
  int i;

  for (i = 0; i < 5000000; i++)
  {
    if (false == bipartite_queue_enqueue(target, &(int){1}))
    {
      fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue value");
      exit(EXIT_FAILURE);
    }
  }

  return NULL;
}

static unsigned long suma = 0;
static unsigned long sumb = 0;

static void *procb(void *arg)
{
  int *item = NULL;

  while (NULL != (item = bipartite_queue_dequeue(target)))
  {
    suma += *item;
  }

  return NULL;
}

static void *procc(void *arg)
{
  int *item = NULL;

  while (NULL != (item = bipartite_queue_dequeue(target)))
  {
    sumb += *item;
  }

  return NULL;
}

static void bipartite_queue_thread_safety_test(void unused **state)
{
  const size_t cap = 40000000;

  pthread_t t1;
  pthread_t t2;
  pthread_t t3;
  pthread_t t4;

  target = bipartite_queue_new(cap, sizeof(int));

  assert_true(pthread_create(&t1, NULL, &proca, NULL) >= 0);
  assert_true(pthread_create(&t2, NULL, &proca, NULL) >= 0);

  assert_true(pthread_join(t1, NULL) >= 0);
  assert_true(pthread_join(t2, NULL) >= 0);

  assert_true(pthread_create(&t3, NULL, &procb, NULL) >= 0);
  assert_true(pthread_create(&t4, NULL, &procc, NULL) >= 0);

  assert_true(pthread_join(t3, NULL) >= 0);
  assert_true(pthread_join(t4, NULL) >= 0);

  bipartite_queue_destroy(target);

  assert_int_equal((suma + sumb), 10000000);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(bipartite_queue_new_test),
    cmocka_unit_test(bipartite_queue_enqueue_test),
    cmocka_unit_test(bipartite_queue_dequeue_test),
    cmocka_unit_test(bipartite_queue_peek_test),
    cmocka_unit_test(bipartite_queue_size_test),
    cmocka_unit_test(bipartite_queue_empty_test),
    cmocka_unit_test(bipartite_queue_thread_safety_test),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
