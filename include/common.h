#ifndef TURNPIKE__COMMON_H
#define TURNPIKE__COMMON_H

#include <stddef.h>
#include <stdlib.h>

static void _free(void **__ptr)
{
  if (NULL != __ptr && NULL != *__ptr)
  {
    free(*__ptr);
    *__ptr = NULL;
  }
}

#define __free(__ptr) _free((void **)&__ptr);

#endif/*TURNPIKE__COMMON_H*/
