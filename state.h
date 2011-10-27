#ifndef STATE_H
#define STATE_H

#include "ant.h"
#include "map.h"

typedef struct food 
{
	int row;
	int col;
} Food;

typedef struct state
{
	Map *map;

	Ant *my_ants;
	guint my_count;

	Ant *enemy_ants;
	guint enemy_count;

	Ant *dead_ants;
	guint dead_count;

	Food *food;
	guint food_count;

	int my_ant_index;
} State;

#endif // STATE_H
