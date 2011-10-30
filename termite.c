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
	int i;

	map_dump (state->map);

	// Chose moves for each ant
	for (i = 0; i < state->my_count; ++i) 
	{
		// the location within the map array where our ant is currently
		Ant *ant = &state->my_ants[i];
		char dir = termite_choose_ant_direction (rules, state, ant);

		// Now we do our move
		if (dir != DIR_NONE)
			termite_move_ant (rules, state, ant, dir);
	}

	// Inform the server we finished sending our actions for the turn
	fprintf (stdout, "go\n");
	fflush (stdout);
	g_debug ("BOT: go\n");
}

// sends a move to the tournament engine and keeps track of ants new location

void termite_move_ant (Rules *rules,
		State* state,
		Ant* ant,
		gchar dir) 
{
	gint old_row = ant_get_row (ant);
	gint old_col = ant_get_col (ant);

	fprintf(stdout, "O %i %i %c\n", old_row, old_col, dir);

	switch (dir) 
	{
		case 'N':
			ant_set_row (ant, old_row - 1);
			break;
		case 'E':
			ant_set_col (ant, old_col + 1);
			break;
		case 'S':
			ant_set_row (ant, old_row + 1);
			break;
		case 'W':
			ant_set_col (ant, old_col - 1);
			break;
	}

	guint n_rows = map_get_n_rows (state->map);
	guint n_cols = map_get_n_cols (state->map);

	if (ant_get_row (ant) == n_rows)
		ant_set_row (ant, 0);
	else if (ant_get_row (ant) == -1)
		ant_set_row (ant, n_rows - 1);

	if (ant_get_col (ant) == n_cols)
		ant_set_col (ant, 0);
	else if (ant_get_col (ant) == -1)
		ant_set_col (ant, n_cols - 1);
}

// initializes the bot on the very first turn using given rules

void termite_init (Rules *rules,
		State *state) 
{
	assert (rules != NULL);
	assert (state != NULL);
	assert (state->map == NULL);

	// Create the empty map
	state->map = map_new (rules->rows, rules->cols, TILE_UNSEEN);

	// Inform the server we're ready to play
	fprintf (stdout, "go\n");
	fflush (stdout);
	g_debug ("BOT: go\n");
}

void termite_cleanup_map (Rules *rules,
		State *state)
{
	guint length = map_get_length (state->map);
	gchar *start = map_get_buffer (state->map);
	gchar *end = start + length;

	do
	{
		gchar tile = *start;
		if (tile != TILE_UNSEEN && tile != TILE_WATER)
			*start = TILE_LAND;
	} while (++start < end);
}

// updates game data with locations of ants and food
// only the ids of your ants are preserved

void termite_update_state (Rules *rules,
		State *state)
{
	Map *map = state->map;
	guint map_len = map_get_length (map);
	gchar *map_data = map_get_buffer (map);
	gchar *map_end = map_data + map_len;
	const guint n_rows = map_get_n_rows (map);
	const guint n_cols = map_get_n_cols (map);

	int my_count = 0;
	int enemy_count = 0;
	int food_count = 0;
	int dead_count = 0;
	static guint ant_id = 0;
	guint i;

	while (map_data < map_end)
	{
		gchar current = *map_data++;

		// Keep them sorted from most to less frequent
		if (current == TILE_UNSEEN || current == TILE_LAND || current == TILE_WATER)
			continue;
		else if (current == TILE_ANT(0))
			++my_count;
		else if (current >= TILE_ANT(1) && current <= TILE_ANT(25))
			++enemy_count;
		else if (current == TILE_FOOD)
			++food_count;
		else if (current == TILE_DEAD_ANT)
			++dead_count;
		else if (current >= TILE_ANT_ON_HILL(1) && current <= TILE_ANT_ON_HILL(25))
			++enemy_count;
		else if (current == TILE_ANT_ON_HILL(0))
			++my_count;
	}

	Ant *my_old = 0;
	int my_old_count = state->my_count;

	state->my_count = my_count;
	state->enemy_count = enemy_count;
	state->food_count = food_count;
	state->dead_count = dead_count;

	if (state->my_ants != 0)
		my_old = state->my_ants;

	if (state->enemy_ants != 0)
		free(state->enemy_ants);
	if (state->food != 0)
		free(state->food);
	if (state->dead_ants != 0)
		free(state->dead_ants);

	state->my_ants = malloc(my_count*sizeof(Ant));

	if (enemy_count > 0)
		state->enemy_ants = malloc(enemy_count*sizeof(Ant));
	else
		state->enemy_ants = 0;

	if (dead_count > 0)
		state->dead_ants = malloc(dead_count*sizeof(Ant));
	else
		state->dead_ants = 0;

	state->food = malloc(food_count*sizeof(struct food));

	gint j;
	for (i = 0; i < n_rows; ++i) 
	{
		for (j = 0; j < n_cols; ++j) 
		{
			char tile = map_get_tile (map, i, j);
			if (tile == TILE_UNSEEN || tile == TILE_LAND || tile == TILE_WATER)
				continue;

			if (tile == TILE_FOOD) 
			{
				--food_count;

				state->food[food_count].row = i;
				state->food[food_count].col = j;
			}
			else if (tile == TILE_ANT(0)) 
			{
				--my_count;

				int keep_id = -1;
				int k = 0;

				if (my_old != 0) 
				{
					for (k = 0; k < my_old_count; ++k) 
					{
						if (my_old[k].row == i && my_old[k].col == j) {
							keep_id = my_old[k].id;
							break;
						}
					}
				}

				state->my_ants[my_count].row = i;
				state->my_ants[my_count].col = j;

				if (keep_id == -1)
					state->my_ants[my_count].id = ++ant_id;
				else
					state->my_ants[my_count].id = keep_id;
			}
			else if (tile == TILE_DEAD_ANT) 
			{
				--dead_count;

				Ant *dead = &state->dead_ants[dead_count];
				ant_set_row (dead, i);
				ant_set_col (dead, j);
				ant_set_owner (dead, tile);
			}
			else 
			{
				--enemy_count;

				Ant *enemy = &state->enemy_ants[enemy_count];
				ant_set_row (enemy, i);
				ant_set_col (enemy, j);
				ant_set_owner (enemy, tile);
			} 
		}
	}

	if (my_old != 0)
		free(my_old);
}

char termite_choose_ant_direction (Rules *rules,
		State *state,
		Ant *ant)
{
	assert (ant != NULL);
	Map *map = state->map;

	char dir = DIR_NONE;
	struct cardinals look = { 0 };

	map_get_cardinals (map, ant_get_row (ant), ant_get_col (ant), &look);

	// cycle through the directions, pick one that works
	if (look.north != '%')
		dir = DIR_NORTH;
	else if (look.east != '%')
		dir = DIR_EAST;
	else if (look.south != '%')
		dir = DIR_SOUTH;
	else if (look.west != '%')
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
		map_set_tile (state->map, row, col, TILE_ANT(owner));
	}
	else if (strcmp (args[0], "f") == 0)
	{
		assert (n_args == 3);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		map_set_tile (state->map, row, col, TILE_FOOD);
	}
	else if (strcmp (args[0], "w") == 0)
	{
		assert (n_args == 3);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		map_set_tile (state->map, row, col, TILE_WATER);
	}
	else if (strcmp (args[0], "h") == 0)
	{
		assert (n_args == 4);
		guint row = atoi (args[1]);
		guint col = atoi (args[2]);
		guint owner = atoi (args[3]);
		map_set_tile (state->map, row, col, TILE_HILL(owner));
	}
	else if (strcmp (args[0], "ready") == 0)
	{
		termite_init (rules, state);
	}
	else if (strcmp (args[0], "turn") == 0)
	{
		assert (n_args == 2);
		state->turn = atoi (args[1]);
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

