#ifndef STATE_H
#define STATE_H

#include "ant.h"
#include "map.h"

typedef struct state
{
	guint n_turn;         // Current turn
	Map *map;

	Tile **ants;
	guint n_ants;

	Ant *enemy_ants;
	guint enemy_count;

	Ant *dead_ants;
	guint dead_count;

	Tile **food;
	guint n_food;
} State;

void state_set_turn (State *state,
		guint n_turn);

guint state_get_turn (State *state);

#endif // STATE_H
