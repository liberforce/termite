#include "utils.h"

// returns the absolute value of a number; used in distance function

int abs (int x) 
{
	if (x >= 0)
		return x;
	return -x;
}

int max (int a, int b)
{
	return (a >= b) ? a : b;
}

int min (int a, int b)
{
	return (a <= b) ? a : b;
}

// returns the distance between two items on the grid accounting for map wrapping

int distance (int row1, int col1, int row2, int col2, struct game_info *Info) 
{
	int dr, dc;
	int abs1, abs2;

	abs1 = abs(row1 - row2);
	abs2 = Info->rows - abs(row1 - row2);
	dr = min (abs1, abs2);

	abs1 = abs(col1 - col2);
	abs2 = Info->cols - abs(col1 - col2);
	dc = min (abs1, abs2);

	return sqrt(pow(dr, 2) + pow(dc, 2));
}

