#include "ant.h"
#include <stdio.h> // for NULL
#include <assert.h>

inline guint8 ant_get_owner (Ant *ant)
{
	assert (ant != NULL);
	return ant->owner;
}

inline gchar ant_get_direction (Ant *ant)
{
	assert (ant != NULL);
	return ant->direction;
}

inline void ant_set_owner (Ant *ant, guint8 player_id)
{
	assert (ant != NULL);
	ant->owner = player_id;
}

inline void ant_set_direction (Ant *ant, gchar direction)
{
	assert (ant != NULL);
	ant->direction = direction;
}
