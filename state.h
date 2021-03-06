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

	Tile **ennemies;
	guint n_ennemies;

	Tile **hills;
	guint n_hills;

	Tile **food;
	guint n_food;

	PathFinder *pf;
	gint64 timer;
} State;

void state_set_turn (State *state,
		guint n_turn);

guint state_get_turn (State *state);

void state_timer_reset (State *state);

gint64 state_timer_get_elapsed (State *state);

#endif // STATE_H
