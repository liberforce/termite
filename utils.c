#include <math.h>
#include <stdlib.h>

#include "utils.h"

// returns the absolute value of a number; used in distance function

inline gint max (gint a, gint b)
{
	return (a >= b) ? a : b;
}

inline gint min (gint a, gint b)
{
	return (a <= b) ? a : b;
}

inline guint distance_sq (guint row1,
		guint col1,
		guint row2,
		guint col2,
		guint n_rows,
		guint n_cols) 
{
	gint dr, dc;
	gint abs1, abs2;

	abs1 = abs(row1 - row2);
	abs2 = n_rows - abs(row1 - row2);
	dr = min (abs1, abs2);

	abs1 = abs(col1 - col2);
	abs2 = n_cols - abs(col1 - col2);
	dc = min (abs1, abs2);

	return dr * dr + dc * dc;
}

// returns the distance between two items on the grid accounting for map wrapping

inline guint distance (guint row1, 
		guint col1, 
		guint row2, 
		guint col2, 
		guint n_rows, 
		guint n_cols) 
{
	return sqrt (distance_sq (row1, 
				col1, 
				row2, 
				col2, 
				n_rows, 
				n_cols));
}

inline guint manhattan_distance (guint row1,
		guint col1,
		guint row2,
		guint col2,
		guint n_rows,
		guint n_cols)
{
	gint dr, dc;
	gint abs1, abs2;

	abs1 = abs(row1 - row2);
	abs2 = n_rows - abs(row1 - row2);
	dr = min (abs1, abs2);

	abs1 = abs(col1 - col2);
	abs2 = n_cols - abs(col1 - col2);
	dc = min (abs1, abs2);

	return dr + dc;
}
