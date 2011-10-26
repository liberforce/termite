#include "ant.h"
#include <stdio.h> // for NULL
#include <assert.h>

inline gint ant_get_col (Ant *ant)
{
	assert (ant != NULL);
	return ant->col;
}

inline gint ant_get_row (Ant *ant)
{
	assert (ant != NULL);
	return ant->row;
}

inline guint ant_get_owner (Ant *ant)
{
	assert (ant != NULL);
	return ant->owner;
}

inline void ant_set_col (Ant *ant, gint col)
{
	assert (ant != NULL);
	ant->col = col;
}

inline void ant_set_row (Ant *ant, gint row)
{
	assert (ant != NULL);
	ant->row = row;
}

inline void ant_set_owner (Ant *ant, guint player_id)
{
	assert (ant != NULL);
	ant->owner = player_id;
}

