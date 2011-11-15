#ifndef TYPES_H
#define TYPES_H

typedef char gchar;
typedef unsigned char guchar;

typedef char gint8;
typedef unsigned char guint8;

typedef int gint;
typedef unsigned int guint;

typedef short gint16;
typedef unsigned short guint16;

typedef int gint32;
typedef unsigned int guint32;

typedef signed long gint64;
typedef unsigned long guint64;

typedef int gboolean;

typedef void * gpointer;

#define TRUE  1
#define FALSE 0

#define G_MAX_UINT ((unsigned int) 0xffffffff)

#define G_LIKELY(expr)  (__builtin_expect (expr, 1))
#define G_UNLIKELY(expr) (__builtin_expect (expr, 0))

#endif // TYPES_H
