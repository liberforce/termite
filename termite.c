#include <stdlib.h>    // for malloc, calloc, free
#include <stdio.h>     // for stdout, stderr, fprintf
#include <string.h>    // for memset, strcmp, strlen
#include <assert.h>    // for assert

#include "termite.h"
#include "ant.h"
#include "map.h"

void termite_do_turn (struct game_state *Game, struct game_info *Info)
{
	int i;

	// Chose moves for each ant
	for (i = 0; i < Game->my_count; ++i) 
	{
		// the location within the map array where our ant is currently
		Ant *ant = &Game->my_ants[i];
		char dir = termite_choose_ant_direction (Game, Info, ant);

		// Now we do our move
		if (dir != DIR_NONE)
			termite_move_ant (Game, Info, ant, dir);
	}
}

// sends a move to the tournament engine and keeps track of ants new location

void termite_move_ant (struct game_state* Game, struct game_info* Info, Ant* ant, char dir) 
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

	guint n_rows = map_get_n_rows (Info->map);
	guint n_cols = map_get_n_cols (Info->map);

	if (ant_get_row (ant) == n_rows)
		ant_set_row (ant, 0);
	else if (ant_get_row (ant) == -1)
		ant_set_row (ant, n_rows - 1);

	if (ant_get_col (ant) == n_cols)
		ant_set_col (ant, 0);
	else if (ant_get_col (ant) == -1)
		ant_set_col (ant, n_cols - 1);
}

// initializes the game_info structure on the very first turn
// function is not called after the game has started

void termite_init_ants (char *data, struct game_info *game_info) 
{
	char *replace_data = data;

	while (*replace_data != '\0')
	{
		if (*replace_data == '\n')
			*replace_data = '\0';
		++replace_data;
	}

	while (42)
	{
		char *value = data;

		while (*++value != ' ');
		++value;

		int num_value = atoi(value);

		switch (*data)
		{
			case 'l':
				game_info->loadtime = num_value;
				break;

			case 't':
				if (*(data + 4) == 't')
					game_info->turntime = num_value;
				else
					game_info->turns = num_value;
				break;

			case 'r':
				game_info->rows = num_value;
				break;

			case 'c':
				game_info->cols = num_value;
				break;

			case 'v':
				game_info->viewradius_sq = num_value;
				break;

			case 'a':
				game_info->attackradius_sq = num_value;
				break;

			case 's':
				if (*(data + 1) == 'p')
					game_info->spawnradius_sq = num_value;
				else
					game_info->seed = num_value;
				break;

		}

		data = value;

		while (*++data != '\0');
		++data;

		if (strcmp(data, "ready") == 0)
			break;
	}
}

// updates game data with locations of ants and food
// only the ids of your ants are preserved

void termite_init_game (struct game_info *game_info, struct game_state *game_state)
{
	Map *map = game_info->map;
	guint map_len = map_get_length (map);
	gchar *map_data = map_get_buffer (map);
	gchar *map_end = map_data + map_len;
	const guint n_rows = map_get_n_rows (map);
	const guint n_cols = map_get_n_cols (map);

	int my_count = 0;
	int enemy_count = 0;
	int food_count = 0;
	int dead_count = 0;
	guint i;

	while (map_data < map_end)
	{
		gchar current = *map_data++;

		if (current == '?' || current == '.' || current == '%')
			continue;
		else if (current == '*')
			++food_count;
		else if (current == 'a')
			++my_count;
		else if (current > 64 && current < 91)
			++dead_count;
		else
			++enemy_count;
	}

	Ant *my_old = 0;
	int my_old_count = game_state->my_count;

	game_state->my_count = my_count;
	game_state->enemy_count = enemy_count;
	game_state->food_count = food_count;
	game_state->dead_count = dead_count;

	if (game_state->my_ants != 0)
		my_old = game_state->my_ants;

	if (game_state->enemy_ants != 0)
		free(game_state->enemy_ants);
	if (game_state->food != 0)
		free(game_state->food);
	if (game_state->dead_ants != 0)
		free(game_state->dead_ants);

	game_state->my_ants = malloc(my_count*sizeof(Ant));

	if (enemy_count > 0)
		game_state->enemy_ants = malloc(enemy_count*sizeof(Ant));
	else
		game_state->enemy_ants = 0;

	if (dead_count > 0)
		game_state->dead_ants = malloc(dead_count*sizeof(Ant));
	else
		game_state->dead_ants = 0;

	game_state->food = malloc(food_count*sizeof(struct food));

	gint j;
	for (i = 0; i < n_rows; ++i) 
	{
		for (j = 0; j < n_cols; ++j) 
		{
			char current = map_get_content (map, i, j);
			if (current == '?' || current == '.' || current == '%')
				continue;

			if (current == '*') 
			{
				--food_count;

				game_state->food[food_count].row = i;
				game_state->food[food_count].col = j;
			}
			else if (current == 'a') 
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

				game_state->my_ants[my_count].row = i;
				game_state->my_ants[my_count].col = j;

				if (keep_id == -1)
					game_state->my_ants[my_count].id = ++game_state->my_ant_index;
				else
					game_state->my_ants[my_count].id = keep_id;
			}
			else if (current > 64 && current < 91) 
			{
				--dead_count;

				Ant *dead = &game_state->dead_ants[dead_count];
				ant_set_row (dead, i);
				ant_set_col (dead, j);
				ant_set_owner (dead, current);
			}
			else 
			{
				--enemy_count;

				Ant *enemy = &game_state->enemy_ants[enemy_count];
				ant_set_row (enemy, i);
				ant_set_col (enemy, j);
				ant_set_owner (enemy, current);
			} 
		}
	}

	if (my_old != 0)
		free(my_old);
}

// Updates the map.
//
//    %   = Walls       (the official spec calls this water,
//                      in either case it's simply space that is occupied)
//    .   = Land        (territory that you can walk on)
//    a   = Your Ant
// [b..z] = Enemy Ants
// [A..Z] = Dead Ants   (disappear after one turn)
//    *   = Food
//    ?   = Unknown     (not used in latest engine version, unknowns are assumed to be land)


void termite_init_map (char *data, struct game_info *game_info) 
{
	Map *map;
	gchar *map_data;
	guint map_len;

	if (game_info->map == NULL)
	{
		game_info->map = map_new (game_info->rows, game_info->cols);
		memset (map_get_buffer (game_info->map), '.', map_get_length (game_info->map));
	}

	map = game_info->map;
	map_data = map_get_buffer (map);
	map_len = map_get_length (map);


	int i;

	for (i = 0; i < map_len; ++i)
		if (map_data[i] != '%')
			map_data[i] = '.';

	while (*data != 0) 
	{
		char *tmp_data = data;
		int arg = 0;

		while (*tmp_data != '\n')
		{
			if (*tmp_data == ' ')
			{
				*tmp_data = '\0';
				++arg;
			}

			++tmp_data;
		}

		char *tmp_ptr = tmp_data;
		tmp_data = data;

		tmp_data += 2;
		int jump = strlen(tmp_data) + 1;

		int row = atoi(tmp_data);
		int col = atoi(tmp_data + jump);
		char var3 = -1;

		if (arg > 2)
		{
			jump += strlen(tmp_data + jump) + 1;
			var3 = *(tmp_data + jump);
		}

		int offset = row * map_get_n_cols (map) + col;

		switch (*data)
		{
			case 'w':
				map_data[offset] = '%';
				break;
			case 'a':
				map_data[offset] = var3 + 49;
				break;
			case 'd':
				map_data[offset] = var3 + 27;
				break;
			case 'f':
				map_data[offset] = '*';
				break;
		}

		data = tmp_ptr + 1;
	}
}

char termite_choose_ant_direction (struct game_state *Game, 
		struct game_info *Info, 
		Ant *ant)
{
	assert (ant != NULL);

	char dir = DIR_NONE;
	struct cardinals look = { 0 };

	map_get_cardinals (Info->map, ant_get_row (ant), ant_get_col (ant), &look);

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

