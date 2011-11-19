#include <stdlib.h>    // for malloc, calloc, free
#include <stdio.h>     // for stdout, stderr, fprintf
#include <string.h>    // for memset, strcmp, strlen
#include <assert.h>    // for assert

#include "termite.h"
#include "ant.h"
#include "direction.h"
#include "map.h"
#include "debug.h"

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

	// 26 players at most
	state->hills = calloc (26, sizeof (Tile *));
	state->n_hills = 0;

	// Our pathfinder
	state->pf = pathfinder_new ();
	pathfinder_set_map (state->pf, state->map);

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

	while (tile < end)
	{
		tile_unset_flag (tile++, TILE_FLAG_HAS_ANT | TILE_FLAG_HAS_FOOD | TILE_FLAG_IS_SEEN | TILE_FLAG_BEING_PROCESSED);
		tile_set_attractivity (tile, 0);
	}

	state->n_ants = 0;
	state->n_food = 0;
	state->n_hills = 0;
}

gchar termite_explore (Rules *rules,
		State *state,
		Tile *tile)
{
	assert (tile != NULL);

	Map *map = state->map;
	gboolean n, s, e, w;
	guint8 n_free_tiles = 0;
	gchar last_dir = ant_get_direction (&tile->with.ant);
	Cardinals look = { NULL, NULL, NULL, NULL };

	map_get_cardinals (map, tile_get_row (tile), tile_get_col (tile), look);

	n = tile_is_free (look[DI_NORTH]);
	s = tile_is_free (look[DI_SOUTH]);
	e = tile_is_free (look[DI_EAST]);
	w = tile_is_free (look[DI_WEST]);
	
	n_free_tiles = n + s +e + w;

	// Pick a direction at random to avoid ants stay blocked on hill
	if (last_dir != DIR_NORTH
			&& last_dir != DIR_SOUTH
			&& last_dir != DIR_EAST
			&& last_dir != DIR_WEST)
		last_dir =  tile_get_random_direction (tile, 25, 25, 25, 25);

	if (n_free_tiles == 4)
	{
		if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 70, 10, 10, 10);
		if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 10, 70, 10, 10);
		if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 10, 10, 70, 10);
		if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 10, 10, 10, 70);
		assert (0);
	}
	else if (n_free_tiles == 3)
	{
		if (n && s && e)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 70, 10, 20, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 10, 70, 20, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 10, 10, 80, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 40, 40, 20, 0);
			assert (0);
		}
		else if (n && s && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 70, 10, 0, 20);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 10, 70, 0, 20);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 40, 40, 0, 20);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 10, 10, 0, 80);
			assert (0);
		}
		else if (s && e && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 20, 40, 40);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 80, 10, 10);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 20, 70, 10);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 20, 10, 70);
			assert (0);
		}
		else if (n && e && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 80, 0, 10, 10);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 20, 0, 40, 40);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 20, 0, 70, 10);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 20, 0, 10, 70);
			assert (0);
		}
	}
	else if (n_free_tiles == 2)
	{
		if (n && s)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 90, 10, 0, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 10, 90, 0, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 50, 50, 0, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 50, 50, 0, 0);
			assert (0);
		}
		else if (e && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 0, 50, 50);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 0, 50, 50);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 0, 90, 10);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 0, 10, 90);
			assert (0);
		}
		else if (e && s)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 10, 90, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 80, 20, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 20, 80, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 90, 10, 0);
			assert (0);
		}
		else if (s && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 10, 0, 90);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 80, 0, 20);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 90, 0, 10);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 20, 0, 80);
			assert (0);
		}
		else if (n && e)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 80, 0, 20, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 10, 0, 90, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 20, 0, 80, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 90, 0, 10, 0);
			assert (0);
		}
		else if (s && e)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 10, 90, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 80, 20, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 20, 80, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 90, 10, 0);
			assert (0);
		}
		else if (n && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 80, 0, 0, 20);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 10, 0, 0, 90);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 90, 0, 0, 10);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 20, 0, 0, 80);
			assert (0);
		}
	}
	else if (n_free_tiles == 1)
	{
		if (n) return DIR_NORTH;
		else if (s) return DIR_SOUTH;
		else if (e) return DIR_EAST;
		else if (w) return DIR_WEST;
		assert (0);
	}

	// Unlikely but may happen when surrounded by ants 
	if (n_free_tiles == 0)
		return DIR_NONE;

	// We should never get here!
	assert (0);
	return DIR_NONE;
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
	if (strcmp (args[0], "a") == 0)
	{
		assert (n_args == 4);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		guint owner = atoi (args[3]);
		Tile *tile = map_get_tile (state->map, row, col);
		tile_set_flag (tile, TILE_FLAG_HAS_ANT);
		ant_set_owner (&tile->with.ant, owner);

		if (owner == 0)
		{
			state->ants[state->n_ants++] = tile;
			tile_set_flag (tile, TILE_FLAG_IS_EXPLORED);
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
		pathfinder_propagate_attractivity (state->pf, tile, 10, 7);
	}
	else if (strcmp (args[0], "w") == 0)
	{
		assert (n_args == 3);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		Tile *tile = map_get_tile (state->map, row, col);
		tile_set_flag (tile, TILE_FLAG_IS_WATER);
	}
	else if (strcmp (args[0], "go") == 0)
	{
		assert (n_args == 1);
		map_dump (state->map);
		map_dump_attractivity (state->map);
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
		Tile *tile = map_get_tile (state->map, row, col);
		tile_set_flag (tile, TILE_FLAG_HAS_HILL);
		tile->with.hill.owner = owner;
		state->hills[state->n_hills++] = tile;
		if (owner != 0)
			pathfinder_propagate_attractivity (state->pf, tile, 40, 7);
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

