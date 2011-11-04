#ifndef STATE_H
#define STATE_H

#include "ant.h"
#include "map.h"
#include "pathfinder.h"

typedef struct state
{
	guint n_turn;         // Current turn
	Map *map;

	Tile **ants;
	guint n_ants;

	Tile **hills;
	guint n_hills;

	Tile **food;
	guint n_food;

	PathFinder *pf;
} State;

void state_set_turn (State *state,
		guint n_turn);

guint state_get_turn (State *state);

#endif // STATE_H
