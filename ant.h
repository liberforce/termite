#ifndef ANT_H
#define ANT_H

#include "types.h"

typedef struct ant 
{
	guint8 owner;
	gchar direction;
} Ant;

guint8 ant_get_owner (Ant *ant);
gchar ant_get_direction (Ant *ant);

void ant_set_owner (Ant *ant, guint8 player_id);
void ant_set_direction (Ant *ant, gchar direction);

#endif // ANT_H
