#include <math.h>
#include <stdlib.h>
#include <assert.h>

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

inline guint euclidian_distance_sq (guint row1,
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

inline guint euclidian_distance (guint row1, 
		guint col1, 
		guint row2, 
		guint col2, 
		guint n_rows, 
		guint n_cols) 
{
	return sqrt (euclidian_distance_sq (row1, 
				col1, 
				row2, 
				col2, 
				n_rows, 
				n_cols));
}

// Shortest wrapped distance between tiles
inline guint manhattan_distance (guint row1,
		guint col1,
		guint row2,
		guint col2,
		guint n_rows,
		guint n_cols)
{
	return (wrapped_distance (row1, row2, n_rows) 
		+ wrapped_distance (col1, col2, n_cols));
}

// Pass all rows, or all cols
inline guint wrapped_distance (guint rc1,
		guint rc2,
		guint n_rcs)
{
	guint d1 = abs ((gint)rc2 - (gint)rc1);
	guint d2 = abs ((rc1 > rc2) ? n_rcs + d1 : d1 - n_rcs);
	return (d1 < d2 ? d1 : d2);
}

// Pass all rows, or all cols
inline gint wrapped_vector (guint rc1,
		guint rc2,
		guint n_rcs)
{
	gint d1 = (gint)rc2 - (gint)rc1;
	gint d2 = (rc1 > rc2) ? n_rcs + d1 : d1 - n_rcs;
	return (abs (d1) < abs (d2)) ? d1 : d2;
}

