#ifndef DEBUG_H
#define DEBUG_H

#ifndef NDEBUG
#define g_here(a) do { fprintf (stderr, "%s:%d", __FILE__, __LINE__); fflush (stderr); } while (0)
#else
#define g_here(a)
#endif

#endif // DEBUG_H
