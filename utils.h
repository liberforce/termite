#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include "types.h"

guint abs (gint x);
gint max (gint a, gint b);
gint min (gint a, gint b);
guint distance (gint row1, gint col1, gint row2, gint col2, gint n_rows, gint n_cols);

#endif // UTILS_H
