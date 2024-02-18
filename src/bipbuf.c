#include "bipbuf.h"
#include "common.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

bipbuf_t *bipbuf_new(const size_t cap)
{
  bipbuf_t *self = NULL;
  self = (bipbuf_t *)_calloc(1, sizeof(*self));
  self->data = (uint8_t *)_calloc(cap, sizeof(*self->data));
  self->cap = cap;
  return self;
}

void __bipbuf_destroy(bipbuf_t **self)
{
  if (self != NULL && *self != NULL)
  {
    __free((*self)->data);
    ___free(*self);
    *self = NULL;
  }
}

bool bipbuf_empty(bipbuf_t *self)
{
  if (self == NULL)
  {
    return false;
  }

  return self->a_start == self->a_end;
}

static size_t bipbuf_unused(bipbuf_t *self)
{
  if (self == NULL)
  {
    return 0;
  }

  if (true == self->b_inuse)
  {
    return self->a_start - self->b_end;
  }

  return self->cap - self->a_end;
}

static size_t bipbuf_used(bipbuf_t *self)
{
  if (self == NULL)
  {
    return 0;
  }

  return (self->a_end - self->a_start) + self->b_end;
}

static void bipbuf_try_switch_to_b(bipbuf_t *self)
{
  if ((self->cap - self->a_end) < (self->a_start - self->b_end))
  {
    self->b_inuse = true;
  }
}

bool bipbuf_offer(bipbuf_t *self, const void *data, const size_t size)
{
  if (self == NULL)
  {
    return false;
  }

  if (bipbuf_unused(self) < size)
  {
    return false;
  }

  if (true == self->b_inuse)
  {
    memcpy((self->data + self->b_end), data, size * sizeof(*self->data));
    self->b_end += size;
  }
  else
  {
    memcpy((self->data + self->a_end), data, size * sizeof(*self->data));
    self->a_end += size;
  }

  bipbuf_try_switch_to_b(self);
  return true;
}

uint8_t *bipbuf_peek(bipbuf_t *self, const size_t size)
{
  if (self == NULL)
  {
    return NULL;
  }

  if (self->cap < (self->a_start + size))
  {
    return NULL;
  }

  if (bipbuf_empty(self))
  {
    return NULL;
  }

  uint8_t *data = NULL;
  data = (uint8_t *)calloc(size, sizeof(*self->data));

  memcpy(data, (self->data + self->a_start), size * sizeof(*self->data));

  return data;
}

uint8_t *bipbuf_poll(bipbuf_t *self, const size_t size)
{
  if (self == NULL)
  {
    return NULL;
  }

  if (bipbuf_empty(self))
  {
    return NULL;
  }

  if (self->cap < (self->a_start + size))
  {
    return NULL;
  }

  void *data = NULL;
  data = (uint8_t *)calloc(size, sizeof(*self->data));

  memcpy(data, (self->data + self->a_start), size * sizeof(*self->data));
  self->a_start += size;

  if (self->a_start == self->a_end)
  {
    if (true == self->b_inuse)
    {
      self->a_start = 0;
      self->a_end = self->b_end;
      self->b_end = 0;
      self->b_inuse = false;
    }
    else
    {
      self->a_start = 0;
      self->a_end = 0;
    }
  }

  bipbuf_try_switch_to_b(self);
  return data;
}
