#include "bipartite.h"

#include <stdio.h>
#include <stdlib.h>

#define QUEUE_CAPACITY       10 * sizeof(int)
#define QUEUE_SEGMENT_LENGTH sizeof(int)

int main(void)
{
  bipartite_queue_t *queue = NULL;
  int i;

  queue = bipartite_queue_new(QUEUE_CAPACITY, QUEUE_SEGMENT_LENGTH);

  for (i = 0; i < 5; i++)
  {
    if (false == bipartite_queue_enqueue(queue, &i))
    {
      fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue value");
      exit(EXIT_FAILURE);
    }
  }

  int sum = 0;
  int *item = NULL;

  while (NULL != (item = bipartite_queue_dequeue(queue)))
  {
    sum += *item;
  }
  printf("%d\n", sum);

  bipartite_queue_destroy(queue);
  return EXIT_SUCCESS;
}
