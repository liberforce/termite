#include <stdlib.h>    // for malloc, calloc, free
#include <stdio.h>     // for stdout, stderr, fprintf
#include <string.h>    // for memset, strcmp, strlen

#include "ants.h"
#include "termite.h"
#include "ant.h"

// The following is an example program displaying a basic ai that
// tries all possible directions and then moves in whichever one
// is not blocked by water (% characters).
//
// To see all the information contained in struct game_state and
// struct game_info check ants.h. There is also a distance function
// provided in bot.c

void termite_do_turn(struct game_state *Game, struct game_info *Info)
{

	// defining things just so we can do less writing
	// UP and DOWN move up and down rows while LEFT and RIGHT
	// move side to side. The map is just one big array.

#define UP -Info->cols
#define DOWN Info->cols
#define LEFT -1
#define RIGHT 1

	int i;

	for (i = 0; i < Game->my_count; ++i) 
	{
		// the location within the map array where our ant is currently
		Ant *ant = &Game->my_ants[i];

		int offset = ant_get_row (ant) * Info->cols + ant_get_col (ant);

		// Now here is the tricky part. We have to account for
		// the fact that the map wraps (when you go off one edge
		// you end up on the side of the map opposite that edge).
		// This is done by checking to see if we are on the last
		// row or column and if the direction we are taking would
		// take us off the side of the map.
		//
		// For example, you can see here the West direction checks
		// to see if we are in the first column, in which case "West"
		// is a character a full row minus one from our location.

		char obj_north, obj_east, obj_south, obj_west;

		if (ant_get_col (ant) != 0)
			obj_west = Info->map[offset + LEFT];
		else
			obj_west = Info->map[offset + Info->cols - 1];

		if (ant_get_col (ant) != Info->cols - 1)
			obj_east = Info->map[offset + RIGHT];
		else
			obj_east = Info->map[offset - Info->cols - 1];

		if (ant_get_row (ant) != 0)
			obj_north = Info->map[offset + UP];
		else
			obj_north = Info->map[offset + (Info->rows - 1)*Info->cols];

		if (ant_get_row (ant) != Info->rows - 1)
			obj_south = Info->map[offset + DOWN];
		else
			obj_south = Info->map[offset - (Info->rows - 1)*Info->cols];

		char dir = -1;

		// cycle through the directions, pick one that works

		if (obj_north != '%')
			dir = 'N';
		else if (obj_east != '%')
			dir = 'E';
		else if (obj_south != '%')
			dir = 'S';
		else if (obj_west != '%')
			dir = 'W';

		// Now we do our move

		if (dir != -1)
			termite_move_ant (Game, Info, ant, dir);
	}

	// There are many ways to make this program better.
	// For starters, try to avoid collisions between your
	// own ants and make a conscious effort to gather food
	// instead of walking around at random.
	//
	// Good luck!
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

	if (ant_get_row (ant) == Info->rows)
		ant_set_row (ant, 0);
	else if (ant_get_row (ant) == -1)
		ant_set_row (ant, Info->rows - 1);

	if (ant_get_col (ant) == Info->cols)
		ant_set_col (ant, 0);
	else if (ant_get_col (ant) == -1)
		ant_set_col (ant, Info->cols - 1);
}

// initializes the game_info structure on the very first turn
// function is not called after the game has started

void termite_init_ants(char *data, struct game_info *game_info) 
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
	int map_len = game_info->rows*game_info->cols;

	int my_count = 0;
	int enemy_count = 0;
	int food_count = 0;
	int dead_count = 0;
	int i, j;

	for (i = 0; i < map_len; ++i)
	{
		char current = game_info->map[i];

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

	for (i = 0; i < game_info->rows; ++i) 
	{
		for (j = 0; j < game_info->cols; ++j) 
		{
			char current = game_info->map[game_info->cols*i + j];
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


void termite_init_map(char *data, struct game_info *game_info) 
{
	if (game_info->map == 0) 
	{
		game_info->map = malloc(game_info->rows*game_info->cols);
		memset(game_info->map, '.', game_info->rows*game_info->cols);
	}

	int map_len = game_info->rows * game_info->cols;
	int i;

	for (i = 0; i < map_len; ++i)
		if (game_info->map[i] != '%')
			game_info->map[i] = '.';

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

		int offset = row*game_info->cols + col;

		switch (*data)
		{
			case 'w':
				game_info->map[offset] = '%';
				break;
			case 'a':
				game_info->map[offset] = var3 + 49;
				break;
			case 'd':
				game_info->map[offset] = var3 + 27;
				break;
			case 'f':
				game_info->map[offset] = '*';
				break;
		}

		data = tmp_ptr + 1;
	}
}
