#ifndef ANT_H
#define ANT_H

#include "types.h"

typedef struct ant 
{
	guint id;
	guint owner;
	gchar direction;
} Ant;

guint ant_get_owner (Ant *ant);
gchar ant_get_direction (Ant *ant);

void ant_set_owner (Ant *ant, guint player_id);
void ant_set_direction (Ant *ant, gchar direction);

#endif // ANT_H
