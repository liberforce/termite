#include <stdlib.h>    // for malloc, calloc, free
#include <stdio.h>     // for stdout, stderr, fprintf
#include <string.h>    // for memset, strcmp, strlen
#include <assert.h>    // for assert

#include "termite.h"
#include "ant.h"
#include "map.h"
#include "tile.h"
#include "debug.h"

void termite_play_turn (Rules *rules,
		State *state)
{
	guint i, j;

	map_dump (state->map);

	// Chose moves for each ant
	for (i = 0; i < state->n_ants; ++i) 
	{
		Tile *ant = state->ants[i];
		Tile *food = NULL;
		gchar dir = DIR_NONE;
		guint distance_sq = G_MAX_UINT;
		gint food_index = -1;
		
		// Find nearest food for that ant
		for (j = 0; j < state->n_food; ++j)
		{
			guint d = map_distance_sq (state->map,
						ant,
						state->food[j]);
			if (d < distance_sq)
			{
				distance_sq = d;
				food = state->food[j];
				food_index = j;
			}
		}

		if (food != NULL)
		{
			g_debug ("ant at [%d,%d] looking for food at [%d,%d]\n",
					ant->row,
					ant->col,
					food->row,
					food->col);

			// Find path to go there !
			if (ant->row < food->row)
				dir = DIR_SOUTH;
			else if (ant->row > food->row)
				dir = DIR_NORTH;
			else if (ant->col < food->col)
				dir = DIR_EAST;
			else if (ant->col > food->col)
				dir = DIR_WEST;

			// "Forget" that food to make sure several ants won't 
			// try to get there at the same time
			// The last item is moved there
			state->food[food_index] = state->food[state->n_food - 1];
			state->n_food--;
		}
		else
		{
			// Exploration
			dir = termite_explore (rules, state, ant);
		}

		// Now we do our move
		if (dir != DIR_NONE)
			termite_move_ant (rules, state, ant, dir);
	}

	state->n_ants = 0;
	state->n_food = 0;

	// Inform the server we finished sending our actions for the turn
	fprintf (stdout, "go\n");
	fflush (stdout);
	g_debug ("BOT: go\n");
}

// sends a move to the tournament engine and keeps track of ants new location

void termite_move_ant (Rules *rules,
		State* state,
		Tile* tile,
		gchar dir) 
{
	assert (tile_get_type (tile) == TILE_TYPE_ANT);
	gint row = tile_get_row (tile);
	gint col = tile_get_col (tile);

	fprintf (stdout, "o %u %u %c\n", row, col, dir);
	g_debug ("BOT: o %u %u %c\n", row, col, dir);
 
	switch (dir)
	{
		case 'N':
			--row;
			break;
		case 'E':
			++col;
			break;
		case 'S':
			++row;
			break;
		case 'W':
			--col;
			break;
	}

	guint n_rows = map_get_n_rows (state->map);
	guint n_cols = map_get_n_cols (state->map);

	if (row == n_rows)
		row = 0;
	else if (row == -1)
		row = n_rows - 1;

	if (col == n_cols)
		col = 0;
	else if (col == -1)
		col = n_cols - 1;

	Tile *next_tile = map_get_tile (state->map, row, col);
	next_tile->with.ant = tile->with.ant;
	tile_set_type (tile, TILE_TYPE_LAND);
}

// initializes the bot on the very first turn using given rules

void termite_init (Rules *rules,
		State *state) 
{
	assert (rules != NULL);
	assert (state != NULL);
	assert (state->map == NULL);

	// Create the empty map
	state->map = map_new (rules->rows, rules->cols, TILE_TYPE_UNSEEN);

	// Allocate max size
	state->ants = calloc (rules->rows * rules->cols, sizeof (Tile *));
	state->n_ants = 0;

	state->food = calloc (rules->rows * rules->cols, sizeof (Tile *));
	state->n_food = 0;

	// Inform the server we're ready to play
	fprintf (stdout, "go\n");
	fflush (stdout);
	g_debug ("BOT: go\n");
}

void termite_cleanup_map (Rules *rules,
		State *state)
{
	Tile *tile = map_get_buffer (state->map);
	Tile *end = tile + map_get_n_elements (state->map);

	do
	{
		if (tile_get_type (tile) != TILE_TYPE_UNSEEN 
				&& tile_get_type (tile) != TILE_TYPE_WATER)
			tile_set_type (tile, TILE_TYPE_LAND);
	} while (++tile < end);
}

gchar termite_explore (Rules *rules,
		State *state,
		Tile *tile)
{
	assert (tile != NULL);
	Map *map = state->map;

	gchar dir = DIR_NONE;
	struct cardinals look = { 0 };

	map_get_cardinals (map, tile_get_row (tile), tile_get_col (tile), &look);

	// cycle through the directions, pick one that works
	if (look.north != TILE_TYPE_WATER)
		dir = DIR_NORTH;
	else if (look.east != TILE_TYPE_WATER)
		dir = DIR_EAST;
	else if (look.south != TILE_TYPE_WATER)
		dir = DIR_SOUTH;
	else if (look.west != TILE_TYPE_WATER)
		dir = DIR_WEST;

	return dir;
}

gboolean termite_process_command (Rules *rules,
		State *state,
		gchar *line)
{
	gboolean game_running = TRUE;
	gchar *arg = line;
	gchar *args[10];
	gint n_args = 0;

	g_debug ("SRV: %s", line);

	// Replace spaces and newlines by null
	// This splits the arguments into multiple null-terminated strings
	while (*arg != '\0')
	{
		args[n_args++] = arg;

		while (*arg != ' ' && *arg != '\n')
			arg++;
		*arg++ = '\0';
	}

	// Determine which action needs to be done
	// Keep them sorted from most to less frequent
	if (strcmp (args[0], "go") == 0)
	{
		assert (n_args == 1);
		termite_play_turn (rules, state);
		termite_cleanup_map (rules, state);
	}
	else if (strcmp (args[0], "a") == 0)
	{
		assert (n_args == 4);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		guint owner = atoi (args[3]);
		Tile *tile = map_get_tile (state->map, row, col);
		tile_set_type (tile, TILE_TYPE_ANT);
		ant_set_owner (&tile->with.ant, owner);

		if (owner == 0)
			state->ants[state->n_ants++] = tile;
	}
	else if (strcmp (args[0], "f") == 0)
	{
		assert (n_args == 3);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		Tile *tile = map_get_tile (state->map, row, col);
		tile_set_type (tile, TILE_TYPE_FOOD);
		state->food[state->n_food++] = tile;
	}
	else if (strcmp (args[0], "w") == 0)
	{
		assert (n_args == 3);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		Tile *tile = map_get_tile (state->map, row, col);
		tile_set_type (tile, TILE_TYPE_WATER);
	}
	else if (strcmp (args[0], "h") == 0)
	{
		assert (n_args == 4);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		guint owner = atoi (args[3]);
		Tile *tile = map_get_tile (state->map, row, col);
		tile_set_type (tile, TILE_TYPE_LAND);
		tile->flags |= TILE_HAS_HILL;
		tile->with.hill.owner = owner;
	}
	else if (strcmp (args[0], "ready") == 0)
	{
		termite_init (rules, state);
	}
	else if (strcmp (args[0], "turn") == 0)
	{
		assert (n_args == 2);
		guint n_turn = atoi (args[1]);
		state_set_turn (state, n_turn);
	}
	else if (strcmp (args[0], "playerseed") == 0)
	{
		assert (n_args == 2);
		rules->seed = atoll (args[1]);
	}
	else if (strcmp (args[0], "attackradius2") == 0)
	{
		assert (n_args == 2);
		rules->attackradius_sq = atoi (args[1]); 
	}
	else if (strcmp (args[0], "spawnradius2") == 0)
	{
		assert (n_args == 2);
		rules->spawnradius_sq = atoi (args[1]); 
		
	}
	else if (strcmp (args[0], "viewradius2") == 0)
	{
		assert (n_args == 2);
		rules->viewradius_sq = atoi (args[1]); 

	}
	else if (strcmp (args[0], "turns") == 0)
	{
		assert (n_args == 2);
		rules->turns = atoi (args[1]); 

	}
	else if (strcmp (args[0], "rows") == 0)
	{
		assert (n_args == 2);
		rules->rows = atoi (args[1]); 
	}
	else if (strcmp (args[0], "cols") == 0)
	{
		assert (n_args == 2);
		rules->cols = atoi (args[1]); 
	}
	else if (strcmp (args[0], "turntime") == 0)
	{
		assert (n_args == 2);
		rules->turntime = atoi (args[1]); 
	}
	else if (strcmp (args[0], "loadtime") == 0)
	{
		assert (n_args == 2);
		rules->loadtime = atoi (args[1]); 
	}
	else if (strcmp (args[0], "end") == 0)
	{
		assert (n_args == 1);
		game_running = FALSE;
	}

	return game_running;
}

