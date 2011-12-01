#include <stdlib.h>    // for malloc, calloc, free
#include <stdio.h>     // for stdout, stderr, fprintf
#include <string.h>    // for memset, strcmp, strlen
#include <assert.h>    // for assert

#include "termite.h"
#include "ant.h"
#include "direction.h"
#include "map.h"
#include "queue.h"
#include "pathfinder.h"
#include "debug.h"

#define HILL_PROPAGATION_DEPTH 14
#define FOOD_PROPAGATION_DEPTH  7

#define MAX_HILLS     26 // 10 (26 in theory) players at most with several hills each

void termite_play_turn (Rules *rules,
		State *state)
{
	guint i;
	gchar dir;

	// Process each ant
	for (i = 0; i < state->n_ants; i++)
	{
		Tile *ant = state->ants[i];

		// Find path to go there !
		dir = pathfinder_get_most_attractive_direction (state->pf, ant); 

		if (dir == DIR_NONE)
		{
			assert (ant != NULL);
			dir = termite_explore (rules, state, ant);
			g_debug ("ant at [%d,%d] wandering towards %c\n",
					ant->row,
					ant->col,
					dir);
		}

		// Now we send our move
		if (dir != DIR_NONE)
		{
			termite_move_ant (rules, state, ant, dir);
		}
	}
}

// Performs checkings required once we have all turn's info
void termite_init_turn (Rules *rules,
		State *state)
{
	assert (rules != NULL);
	assert (state != NULL);

	guint i = 0;

	// Check for razed hills
	// At this time, all seen hills have been announced
	while (i < state->n_hills)
	{
		Tile *t = state->hills[i];

		if G_UNLIKELY (! tile_is_flag_set (t, TILE_FLAG_HAS_HILL))
		{
			// The hill has somehow been razed, as it was 
			// not signaled this turn
			state->hills[i] = state->hills[--state->n_hills];
			continue;
		}

		// Propagate ennemy hill scent
		if (hill_get_owner (&t->with.hill) != 0)
		{
			pathfinder_propagate_attractivity (state->pf,
					t,
					40,
					-1,
					HILL_PROPAGATION_DEPTH);
		}

		i++;
	}

	// Count how many times a tile is seen by different friendly ants
	i = 0;
	while (i < state->n_ants)
	{
		Tile *t = state->ants[i];
		Queue *queue = pathfinder_select_in_range_sq (state->pf,
				t,
				rules->viewradius_sq);

		while (! queue_is_empty (queue))
		{
			t = queue_pop (queue);
			tile_incr_seen (t);
			tile_unset_flag (t, TILE_FLAG_BEING_PROCESSED);
			if G_UNLIKELY (! tile_is_flag_set (t, TILE_FLAG_IS_EXPLORED))
				tile_set_flag (t, TILE_FLAG_IS_EXPLORED);
		}
		i++;
	}
}

// sends a move to the tournament engine and keeps track of ants new location
void termite_move_ant (Rules *rules,
		State* state,
		Tile* tile,
		gchar dir) 
{
	assert (tile_is_flag_set (tile, TILE_FLAG_HAS_ANT));
	guint row = tile_get_row (tile);
	guint col = tile_get_col (tile);

	fprintf (stdout, "o %u %u %c\n", row, col, dir);
	// g_debug ("BOT: o %u %u %c\n", row, col, dir);
 
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

	if G_UNLIKELY (row == n_rows)
		row = 0;
	else if G_UNLIKELY (row == G_MAX_UINT)
		row = n_rows - 1;

	if G_UNLIKELY (col == n_cols)
		col = 0;
	else if G_UNLIKELY (col == G_MAX_UINT)
		col = n_cols - 1;

	// The ant moved out of that tile...
	tile_unset_flag (tile, TILE_FLAG_HAS_ANT);

	// ...to go on that one
	Tile *next_tile = map_get_tile (state->map, row, col);
	assert (! tile_is_flag_set (next_tile, TILE_FLAG_HAS_ANT));
	tile_set_flag (next_tile, TILE_FLAG_HAS_ANT);
	next_tile->with.ant = tile->with.ant;

	// Remember chosen direction (useful for exploration)
	ant_set_direction (&next_tile->with.ant, dir);
}

// initializes the bot on the very first turn using given rules

void termite_init (Rules *rules,
		State *state) 
{
	assert (rules != NULL);
	assert (state != NULL);
	assert (state->map == NULL);

	// Create the empty map
	state->map = map_new (rules->rows, rules->cols);

	// Allocate max size
	state->ants = calloc (rules->rows * rules->cols, sizeof (Tile *));
	state->n_ants = 0;

	state->food = calloc (rules->rows * rules->cols, sizeof (Tile *));
	state->n_food = 0;

	state->ennemies = calloc (rules->rows * rules->cols, sizeof (Tile *));
	state->n_ennemies = 0;

	state->hills = calloc (MAX_HILLS, sizeof (Tile *));
	state->n_hills = 0;

	// Our pathfinder
	state->pf = pathfinder_new ();
	pathfinder_set_map (state->pf, state->map);

	// Inform the server we're ready to play
	fprintf (stdout, "go\n");
	fflush (stdout);
	// g_debug ("BOT: go\n");
}

void termite_cleanup_map (Rules *rules,
		State *state)
{
	Tile *tile = map_get_buffer (state->map);
	Tile *end = tile + map_get_n_elements (state->map);

	// Prepare next turn's check for razed hills
	guint i;
	for (i = 0; i < state->n_hills; i++)
	{
		Tile *t = state->hills[i];

		// If an hill is supposed to be seen on next turn, then if it's
		// not announced, that means the hill has been razed 
		if (tile_is_seen (t))
		{
			tile_unset_flag (t, TILE_FLAG_HAS_HILL);
		}
	}

	while (tile < end)
	{
		tile_unset_flag (tile, TILE_FLAG_HAS_ANT | TILE_FLAG_HAS_FOOD);
		tile_set_seen (tile, 0);
		tile_set_attractivity (tile, 0);
		++tile;
	}

	state->n_ants = 0;
	state->n_ennemies = 0;
	state->n_food = 0;
}

gchar termite_explore (Rules *rules,
		State *state,
		Tile *tile)
{
	return pathfinder_explore_random (state->pf, tile);
}

gboolean termite_process_command (Rules *rules,
		State *state,
		gchar *line)
{
	gboolean game_running = TRUE;
	gchar *arg = line;
	gchar *args[10];
	gint n_args = 0;

	// g_debug ("SRV: %s", line);

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
	if (strcmp (args[0], "a") == 0)
	{
		assert (n_args == 4);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		guint owner = atoi (args[3]);
		Tile *tile = map_get_tile (state->map, row, col);
		tile_set_flag (tile, TILE_FLAG_HAS_ANT);
		ant_set_owner (&tile->with.ant, owner);

		if G_LIKELY (owner == 0)
		{
			state->ants[state->n_ants++] = tile;
		}
		else
		{
			state->ennemies[state->n_ennemies++] = tile;
		}
	}
	else if (strcmp (args[0], "f") == 0)
	{
		assert (n_args == 3);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		Tile *tile = map_get_tile (state->map, row, col);
		tile_set_flag (tile, TILE_FLAG_HAS_FOOD);
		state->food[state->n_food++] = tile;
		pathfinder_propagate_attractivity (state->pf,
				tile,
				10,
				-1,
				FOOD_PROPAGATION_DEPTH);
	}
	else if (strcmp (args[0], "w") == 0)
	{
		assert (n_args == 3);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		Tile *tile = map_get_tile (state->map, row, col);
		tile_set_flag (tile, TILE_FLAG_IS_WATER);
	}
	else if (strcmp (args[0], "d") == 0)
	{
		assert (n_args == 4);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		// guint owner = atoi (args[3]);
		Tile *tile = map_get_tile (state->map, row, col);
		tile_unset_flag (tile, TILE_FLAG_HAS_ANT);
	}
	else if (strcmp (args[0], "go") == 0)
	{
		assert (n_args == 1);

		// Perform checkings required once we have all turn's info
		termite_init_turn (rules, state);

		//map_dump (state->map);
		//map_dump_attractivity (state->map);
		g_debug ("%06li: map_dump\n", state_timer_get_elapsed (state));

		termite_play_turn (rules, state);
		g_debug ("%06li: termite_play_turn\n", state_timer_get_elapsed (state));

		// Inform the server we finished sending our actions for the turn
		fprintf (stdout, "go\n");
		fflush (stdout);
		g_debug ("%06li: go sent\n", state_timer_get_elapsed (state));

		termite_cleanup_map (rules, state);
		g_debug ("%06li: termite_cleanup_map\n", state_timer_get_elapsed (state));
	}
	else if (strcmp (args[0], "h") == 0)
	{
		assert (n_args == 4);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		guint owner = atoi (args[3]);
		gboolean is_new_hill = TRUE;
		Tile *tile = map_get_tile (state->map, row, col);

		// Find out if this hill is already known
		guint i = 0;
		for (i = 0; i < state->n_hills; i++)
		{
			if (tile == state->hills[i])
			{
				is_new_hill = FALSE;
				break;
			}
		}

		// Detect new hills
		if (is_new_hill)
		{
			assert (state->n_hills < MAX_HILLS);
			tile->with.hill.owner = owner;
			state->hills[state->n_hills++] = tile;
		}

		// Remember we've seen this hill this turn
		tile_set_flag (tile, TILE_FLAG_HAS_HILL);
	}
	else if (strcmp (args[0], "turn") == 0)
	{
		assert (n_args == 2);
		guint n_turn = atoi (args[1]);
		g_debug ("turn %d\n", n_turn);
		state_set_turn (state, n_turn);
	}
	else if (strcmp (args[0], "ready") == 0)
	{
		termite_init (rules, state);
	}
	else if (strcmp (args[0], "playerseed") == 0)
	{
		assert (n_args == 2);
		rules->seed = atoll (args[1]);
		srand (rules->seed);
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

