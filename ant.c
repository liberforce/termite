#include "ant.h"
#include <assert.h>

inline int ant_get_col (Ant *ant)
{
	assert (ant != NULL);
	return ant->col;
}

inline int ant_get_row (Ant *ant)
{
	assert (ant != NULL);
	return ant->row;
}

inline int ant_set_col (Ant *ant, int col)
{
	assert (ant != NULL);
	return ant->col = col;
}

inline void ant_set_row (Ant *ant, int row)
{
	assert (ant != NULL);
	return ant->row = row;
}

