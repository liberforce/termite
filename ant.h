#ifndef ANT_H
#define ANT_H

#include "types.h"

typedef struct ant 
{
	int id;
	int row;
	int col;
	int owner;
} Ant;

gint ant_get_col (Ant *ant);
gint ant_get_row (Ant *ant);
guint ant_get_owner (Ant *ant);

void ant_set_col (Ant *ant, gint col);
void ant_set_row (Ant *ant, gint row);
void ant_set_owner (Ant *ant, guint player_id);

#endif // ANT_H
