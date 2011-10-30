#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "rules.h"
#include "state.h"
#include "termite.h"

// main, communicates with tournament engine

int main (int argc, char *argv[]) 
{
	gboolean game_running = TRUE;
	gchar input_buffer[1024];
	gint length = sizeof (input_buffer) / sizeof (*input_buffer);
	gchar *line;
	Rules rules;
	State state;

	memset (&rules, 0, sizeof (rules));
	memset (&state, 0, sizeof (state));

	while (game_running) 
	{
		if ((line = fgets (input_buffer, length, stdin)) == NULL)
			continue;

		game_running = termite_process_command (&rules, &state, line);
	}

	return 0;
}

