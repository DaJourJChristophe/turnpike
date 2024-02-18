#ifndef TURNPIKE__COMMON_H
#define TURNPIKE__COMMON_H

#include <jemalloc/jemalloc.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef always_inline
#define always_inline __attribute__ ((always_inline))
#endif/*always_inline*/

static void __die(const char *funcname, const char *msg)
{
  const char fmt[] = "%s(): %s\n";
  fprintf(stderr, fmt, funcname, msg);
  exit(EXIT_FAILURE);
}

#define die(__msg) __die(__func__, __msg)

static void *_calloc(const size_t nmemb, const size_t size)
{
  void *__ptr = NULL;
  __ptr = mallocx((nmemb * size), MALLOCX_ZERO);
  if (__ptr == NULL)
  {
    die("a memory error occurred");
  }
  return __ptr;
}

static void ___free(void *__ptr)
{
  dallocx(__ptr, 0);
}

static void _free(void **__ptr)
{
  if (NULL != __ptr && NULL != *__ptr)
  {
    ___free(*__ptr);
    *__ptr = NULL;
  }
}

#define __free(__ptr) _free((void **)&__ptr);

#endif/*TURNPIKE__COMMON_H*/
