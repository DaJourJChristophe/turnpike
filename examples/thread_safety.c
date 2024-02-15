#include "tsqueue.h"

#include <stdio.h>
#include <stdlib.h>

#define QUEUE_CAPACITY 10

int main(void)
{
  ts_queue_t *queue = NULL;
  int i;

  queue = ts_queue_new(QUEUE_CAPACITY);

  for (i = 0; i < 5; i++)
  {
    if (false == ts_queue_enqueue(queue, i))
    {
      fprintf(stderr, "%s(): %s", __func__, "could not enqueue value");
      exit(EXIT_FAILURE);
    }
  }

  int sum = 0;
  int *item = NULL;

  while (NULL != (item = ts_queue_dequeue(queue)))
  {
    sum += *item;
  }
  printf("%d\n", sum);

  ts_queue_destroy(queue);
  return EXIT_SUCCESS;
}
