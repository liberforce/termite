#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h> // for sigaction

#include "types.h"
#include "rules.h"
#include "state.h"
#include "termite.h"

// main, communicates with tournament engine

static State *s = NULL;

void on_signal_continue (int num, siginfo_t *info, void *context)
{
	state_timer_reset (s);
}

int main (int argc, char *argv[]) 
{
	gboolean game_running = TRUE;
	gchar input_buffer[1024];
	gint length = sizeof (input_buffer) / sizeof (*input_buffer);
	gchar *line;
	Rules rules;
	State state;
	s = &state;

	memset (&rules, 0, sizeof (rules));
	memset (&state, 0, sizeof (state));

	// Intercept SIGCONT to measure turn time
	struct sigaction newsig;
	newsig.sa_sigaction = on_signal_continue;
	sigemptyset (&newsig.sa_mask);
	newsig.sa_flags = SA_SIGINFO | SA_NODEFER;
	sigaction (SIGCONT, &newsig, NULL);

	while (game_running) 
	{
		if ((line = fgets (input_buffer, length, stdin)) == NULL)
			continue;

		game_running = termite_process_command (&rules, &state, line);
	}

	return 0;
}

