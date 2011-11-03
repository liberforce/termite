#ifndef UTILS_H
#define UTILS_H

#include "types.h"

gint max (gint a, gint b);
gint min (gint a, gint b);

guint distance (guint row1,
		guint col1,
		guint row2,
		guint col2,
		guint n_rows,
		guint n_cols);

guint distance_sq (guint row1,
		guint col1,
		guint row2,
		guint col2,
		guint n_rows,
		guint n_cols);

guint manhattan_distance (guint row1,
		guint col1,
		guint row2,
		guint col2,
		guint n_rows,
		guint n_cols);

#endif // UTILS_H
