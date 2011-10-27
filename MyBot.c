#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rules.h"
#include "termite.h"
#include "debug.h"

void on_init_game (Rules *rules, State *state)
{
	termite_init_game (rules, state);
}

void on_init_map (char *data, Rules *rules, State *state)
{
	termite_init_map (data, state);
}

void on_init_bot (Rules *rules, State *state, char *data)
{
	termite_init (rules, state, data);
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

	Rules rules;
	State state;

	state.map = NULL;
	state.my_ants = NULL;
	state.enemy_ants = NULL;
	state.food = NULL;
	state.dead_ants = NULL;

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

			on_init_map(skip_line, &rules, &state);
			on_init_game(&rules, &state);
			termite_do_turn(&state, &rules);
			fprintf(stdout, "go\n");
			fflush(stdout);
		}
		else if (action == 1)
		{
			on_init_bot (&rules, &state, data + 1);

			state.my_ant_index = -1;

			fprintf(stdout, "go\n");
			fflush(stdout);
		}

		free(data);
	}
}
