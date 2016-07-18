#ifndef __utils_h__
#define __utils_h__

#include <stdio.h>

#define derror(format, ...) do { fprintf (stderr, format, ## __VA_ARGS__); } while (0)
#define dexit(format, ...) do { fprintf (stderr, format, ## __VA_ARGS__); exit(1); } while (0)

#endif /* __utils_h__ */
