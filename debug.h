#ifndef DEBUG_H
#define DEBUG_H

#ifndef NDEBUG
#define g_here(a) do { fprintf (stderr, "%s:%d\n", __FILE__, __LINE__); fflush (stderr); } while (0)
#define g_debug(...) do { fprintf (stderr, __VA_ARGS__); fflush (stderr); } while (0)
#else
#define g_here(a)
#define g_debug(...)
#endif

#endif // DEBUG_H
