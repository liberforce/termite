#include <stdlib.h>    // for malloc, calloc, free
#include <stdio.h>     // for stdout, stderr, fprintf
#include <string.h>    // for memset, strcmp, strlen
#include <assert.h>    // for assert

#include "termite.h"
#include "ant.h"
#include "map.h"
#include "debug.h"

void termite_play_turn (Rules *rules,
		State *state)
{
	guint i;

	while (state->n_ants > 0)
	{
		gchar dir = DIR_NONE;
		guint distance; // Manhattan distance
		Tile *food = NULL;
		Tile *ant = NULL;
		gint ant_index = -1;

		// Find nearest ant for that food
		if (state->n_food > 0)
			food = state->food[state->n_food - 1];

		if (food != NULL)
		{
			distance = G_MAX_UINT;
			for (i = 0; i < state->n_ants; ++i) 
			{
				guint d = map_get_manhattan_distance (state->map,
						state->ants[i],
						food);
				if (d < distance)
				{
					distance = d;
					ant = state->ants[i];
					ant_index = i;
				}
			}
		}

		if (ant != NULL)
		{
			gint food_index = -1;

			// Find nearest food for that ant
			distance = G_MAX_UINT;
			for (i = 0; i < state->n_food; ++i) 
			{
				guint d = map_get_manhattan_distance (state->map,
						ant,
						state->food[i]);
				if (d < distance)
				{
					distance = d;
					food = state->food[i];
					food_index = i;
				}
			}

			g_debug ("ant at [%d,%d] looking for food at [%d,%d]\n",
					ant->row,
					ant->col,
					food->row,
					food->col);

			struct cardinals look = { 0 };
			map_get_cardinals (state->map, tile_get_row (ant), tile_get_col (ant), &look);

			// Find path to go there !
			if (ant->row < food->row && tile_is_free (look.south))
				dir = DIR_SOUTH;
			else if (ant->row > food->row && tile_is_free (look.north))
				dir = DIR_NORTH;
			else if (ant->col < food->col && tile_is_free (look.east))
				dir = DIR_EAST;
			else if (ant->col > food->col && tile_is_free (look.west))
				dir = DIR_WEST;

			// "Forget" that food to make sure several ants won't 
			// try to get there at the same time
			if (dir != DIR_NONE)
			{
				assert (food_index >= 0);
				state->food[food_index] = state->food[state->n_food - 1];
				state->n_food--;
			}
		}
		else
		{
			// Exploration
			ant_index = state->n_ants - 1;
			ant = state->ants[ant_index];
			dir = termite_explore (rules, state, ant);
			g_debug ("ant at [%d,%d] wandering towards %c\n",
					ant->row,
					ant->col,
					dir);
		}

		// Now we do our move
		if (dir != DIR_NONE)
		{
			termite_move_ant (rules, state, ant, dir);
		}

		assert (ant_index >= 0);
		state->ants[ant_index] = state->ants[state->n_ants - 1];
		state->n_ants--;
	}

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

	tile_set_type (tile, TILE_TYPE_LAND);

	Tile *next_tile = map_get_tile (state->map, row, col);
	assert (tile_get_type (next_tile) != TILE_TYPE_ANT);
	tile_set_type (next_tile, TILE_TYPE_ANT);
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
	state->map = map_new (rules->rows, rules->cols, TILE_TYPE_UNSEEN);

	// Allocate max size
	state->ants = calloc (rules->rows * rules->cols, sizeof (Tile *));
	state->n_ants = 0;

	state->food = calloc (rules->rows * rules->cols, sizeof (Tile *));
	state->n_food = 0;

	// 26 players at most
	state->hills = calloc (26, sizeof (Tile *));
	state->n_hills = 0;

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
		{
			tile_set_type (tile, TILE_TYPE_LAND);
			tile_set_flags (tile, tile_get_flags (tile) & ~TILE_HAS_HILL);
		}
	} while (++tile < end);

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
	struct cardinals look = { NULL };

	map_get_cardinals (map, tile_get_row (tile), tile_get_col (tile), &look);

	n = tile_is_free (look.north);
	s = tile_is_free (look.south);
	e = tile_is_free (look.east);
	w = tile_is_free (look.west);
	
	n_free_tiles = n + s +e + w;

	if (n_free_tiles == 4)
	{
		if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 7, 1, 1, 1);
		if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 1, 7, 1, 1);
		if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 1, 1, 7, 1);
		if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 1, 1, 1, 7);
	}
	else if (n_free_tiles == 3)
	{
		if (n && s && e)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 7, 1, 2, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 1, 7, 2, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 1, 1, 8, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 4, 4, 2, 0);
		}
		else if (n && s && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 7, 1, 0, 2);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 1, 7, 0, 2);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 4, 4, 0, 2);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 1, 1, 0, 8);
		}
		else if (s && e && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 2, 4, 4);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 8, 1, 1);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 2, 7, 1);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 2, 1, 7);
		}
		else if (n && e && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 8, 0, 1, 1);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 2, 0, 4, 4);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 2, 0, 7, 1);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 2, 0, 1, 7);
		}
	}
	else if (n_free_tiles == 2)
	{
		if (n && s)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 9, 1, 0, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 1, 9, 0, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 5, 5, 0, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 5, 5, 0, 0);
		}
		else if (e && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 0, 5, 5);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 0, 5, 5);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 0, 9, 1);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 0, 1, 9);
		}
		else if (e && s)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 1, 9, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 8, 2, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 2, 8, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 9, 1, 0);
		}
		else if (s && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 1, 0, 9);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 8, 0, 2);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 9, 0, 1);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 2, 0, 8);
		}
		else if (n && e)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 8, 0, 2, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 1, 0, 9, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 2, 0, 8, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 9, 0, 1, 0);
		}
		else if (s && e)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 1, 9, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 8, 2, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 2, 8, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 9, 1, 0);
		}
		else if (n && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 8, 0, 0, 2);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 1, 0, 0, 9);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 9, 0, 0, 1);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 2, 0, 0, 8);
		}
	}
	else if (n_free_tiles == 1)
	{
		if (n) return DIR_NORTH;
		else if (s) return DIR_SOUTH;
		else if (e) return DIR_EAST;
		else if (w) return DIR_WEST;
	}

	// Unlikely but may happen when surrounded by ants 
	// if (n_free_tiles == 0)
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
	if (strcmp (args[0], "go") == 0)
	{
		assert (n_args == 1);
		map_dump (state->map);
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
		state->hills[state->n_hills++] = tile;
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

