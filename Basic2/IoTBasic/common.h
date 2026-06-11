/*
 * a few common definitions, mostly compiler stuff
 */

/* portable intentional-fall-through marker for switch statements */
#ifndef FALLTHROUGH
#if defined(__GNUC__) && __GNUC__ >= 7
#define FALLTHROUGH __attribute__((fallthrough))
#elif defined(__cplusplus) && __cplusplus >= 201703L
#define FALLTHROUGH [[fallthrough]]
#else
#define FALLTHROUGH ((void)0)
#endif
#endif

