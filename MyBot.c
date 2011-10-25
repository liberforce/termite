#include "ants.h"
#include "ant.h"

// returns the absolute value of a number; used in distance function

int abs(int x) 
{
	if (x >= 0)
		return x;
	return -x;
}

int max (int a, int b)
{
	return (a >= b) ? a : b;
}

int min (int a, int b)
{
	return (a <= b) ? a : b;
}

// returns the distance between two items on the grid accounting for map wrapping

int distance (int row1, int col1, int row2, int col2, struct game_info *Info) 
{
	int dr, dc;
	int abs1, abs2;

	abs1 = abs(row1 - row2);
	abs2 = Info->rows - abs(row1 - row2);
	dr = min (abs1, abs2);

	abs1 = abs(col1 - col2);
	abs2 = Info->cols - abs(col1 - col2);
	dc = min (abs1, abs2);

	return sqrt(pow(dr, 2) + pow(dc, 2));
}

// sends a move to the tournament engine and keeps track of ants new location

void bot_move_ant (struct game_state* Game, struct game_info* Info, Ant* ant, char dir) 
{
	fprintf(stdout, "O %i %i %c\n", ant->row, ant->col, dir);

	switch (dir) 
	{
		case 'N':
			ant->row -= 1;
			break;
		case 'E':
			ant->col += 1;
			break;
		case 'S':
			ant->row += 1;
			break;
		case 'W':
			ant->col -= 1;
			break;
	}

	if (ant->row == Info->rows)
		ant->row = 0;
	else if (ant->row == -1)
		ant->row = Info->rows - 1;

	if (ant->col == Info->cols)
		ant->col = 0;
	else if (ant->col == -1)
		ant->col = Info->cols - 1;
}

// just a function that returns the string on a given line for i/o
// you don't need to worry about this

char *get_line(char *text)
{
	char *tmp_ptr = text;
	int len = 0;

	while (*tmp_ptr != '\n') 
	{
		++tmp_ptr;
		++len;
	}

	char *return_str = malloc(len + 1);
	memset(return_str, 0, len + 1);

	int i;
	for (i = 0; i < len; ++i) 
	{
		return_str[i] = text[i];
	}

	return return_str;
}

// main, communicates with tournament engine

int main(int argc, char *argv[]) 
{
	int action = -1;

	struct game_info Info;
	struct game_state Game;
	Info.map = 0;

	Game.my_ants = 0;
	Game.enemy_ants = 0;
	Game.food = 0;
	Game.dead_ants = 0;

	while (42) 
	{
		int initial_buffer = 100000;

		char *data = malloc(initial_buffer);
		memset(data, 0, initial_buffer);

		*data = '\n';

		char *ins_data = data + 1;

		int i = 0;

		while (1 > 0) 
		{
			++i;

			if (i > initial_buffer) 
			{
				initial_buffer *= 2;
				data = realloc(data, initial_buffer);
				memset(ins_data, 0, initial_buffer/2);
			}

			*ins_data = getchar();

			if (*ins_data == '\n') 
			{
				char *backup = ins_data;

				while (*(backup - 1) != '\n') 
				{
					--backup;
				}

				char *test_cmd = get_line(backup);

				if (strcmp(test_cmd, "go") == 0)
				{
					action = 0; 
					free(test_cmd);
					break;
				}
				else if (strcmp(test_cmd, "ready") == 0)
				{
					action = 1;
					free(test_cmd);
					break;
				}
				free(test_cmd);
			}

			++ins_data;
		}

		if (action == 0)
		{
			char *skip_line = data + 1;
			while (*++skip_line != '\n');
			++skip_line;

			_init_map(skip_line, &Info);
			_init_game(&Info, &Game);
			do_turn(&Game, &Info);
			fprintf(stdout, "go\n");
			fflush(stdout);
		}
		else if (action == 1)
		{
			_init_ants(data + 1, &Info);

			Game.my_ant_index = -1;

			fprintf(stdout, "go\n");
			fflush(stdout);
		}

		free(data);
	}
}
