
#ifndef sodium_core_H
#define sodium_core_H

#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
int sodium_init(void)
            __attribute__ ((warn_unused_result));

/* ---- */

SODIUM_EXPORT
int sodium_set_misuse_handler(void (*handler)(void));

SODIUM_EXPORT
void sodium_misuse(void)
            __attribute__ ((noreturn));

/* IOTA -- Port to CubeIDE */
//#ifdef __GNUC__
#if (defined(__GNUC__) && !defined(__CC_ARM))
__attribute__((weak)) void
__aeabi_assert(char const *test, char const *file, int line)
{
  (void)test;
  (void)file;
  (void)line;
}

__attribute__((weak)) int
volatile *__aeabi_errno_addr(void)
{
  return NULL;
}
#endif

#ifdef __cplusplus
}
#endif

#endif
