#include "queue.h"

#include <stdio.h>
#include <stdlib.h>

#define QUEUE_CAPACITY 10

int main(void)
{
  queue_t *queue = NULL;
  int i;

  queue = queue_new(QUEUE_CAPACITY);

  for (i = 0; i < 5; i++)
  {
    if (false == queue_enqueue(queue, i))
    {
      fprintf(stderr, "%s(): %s", __func__, "could not enqueue value");
      exit(EXIT_FAILURE);
    }
  }

  int sum = 0;
  int *item = NULL;

  while (NULL != (item = queue_dequeue(queue)))
  {
    sum += *item;
  }
  printf("%d\n", sum);

  queue_destroy(queue);
  return EXIT_SUCCESS;
}
