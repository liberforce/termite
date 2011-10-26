#include "utils.h"

// returns the absolute value of a number; used in distance function

guint abs (gint x) 
{
	return (x >= 0) ? x : -x;
}

gint max (gint a, gint b)
{
	return (a >= b) ? a : b;
}

gint min (gint a, gint b)
{
	return (a <= b) ? a : b;
}

// returns the distance between two items on the grid accounting for map wrapping

guint distance (gint row1, gint col1, gint row2, gint col2, gint n_rows, gint n_cols) 
{
	gint dr, dc;
	gint abs1, abs2;

	abs1 = abs(row1 - row2);
	abs2 = n_rows - abs(row1 - row2);
	dr = min (abs1, abs2);

	abs1 = abs(col1 - col2);
	abs2 = n_cols - abs(col1 - col2);
	dc = min (abs1, abs2);

	return sqrt(dr * dr + dc * dc);
}

