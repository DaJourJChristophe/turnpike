#ifndef TURNPIKE_BIPBUF_H
#define TURNPIKE_BIPBUF_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

struct bipbuf
{
  uint8_t *data;
  size_t cap;
  uint64_t a_start;
  uint64_t a_end;
  uint64_t b_end;
  bool b_inuse;
};

typedef struct bipbuf bipbuf_t;

bipbuf_t *bipbuf_new(const size_t cap);

void __bipbuf_destroy(bipbuf_t **self);

#define bipbuf_destroy(self) __bipbuf_destroy(&self)

bool bipbuf_empty(bipbuf_t *self);

bool bipbuf_offer(bipbuf_t *self, const void *data, const size_t size);

uint8_t *bipbuf_peek(bipbuf_t *self, const size_t size);

uint8_t *bipbuf_poll(bipbuf_t *self, const size_t size);

#endif/*TURNPIKE_BIPBUF_H*/
