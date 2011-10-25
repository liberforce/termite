#include "ant.h"
#include <stdio.h> // for NULL
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

inline int ant_get_owner (Ant *ant)
{
	assert (ant != NULL);
	return ant->owner;
}

inline void ant_set_col (Ant *ant, int col)
{
	assert (ant != NULL);
	ant->col = col;
}

inline void ant_set_row (Ant *ant, int row)
{
	assert (ant != NULL);
	ant->row = row;
}

inline void ant_set_owner (Ant *ant, int player_id)
{
	assert (ant != NULL);
	ant->owner = player_id;
}

