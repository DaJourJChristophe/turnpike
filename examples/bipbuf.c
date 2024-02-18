#include "bipbuf.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  bipbuf_t *buffer = NULL;
  buffer = bipbuf_new(8);

  if (false == bipbuf_offer(buffer, &(int){5}, sizeof(int)))
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not offer data to buffer");
    exit(EXIT_FAILURE);
  }
  if (false == bipbuf_offer(buffer, &(int){7}, sizeof(int)))
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not offer data to buffer");
    exit(EXIT_FAILURE);
  }

  int *data = NULL;
  data = (int *)bipbuf_poll(buffer, sizeof(int));

  if (data != NULL)
  {
    printf("%d\n", *data);
    free(data); data = NULL;
  }

  data = (int *)bipbuf_poll(buffer, sizeof(int));

  if (data != NULL)
  {
    printf("%d\n", *data);
    free(data); data = NULL;
  }

  bipbuf_destroy(buffer);

  return EXIT_SUCCESS;
}
