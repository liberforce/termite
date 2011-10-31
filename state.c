#include <assert.h>
#include <stdlib.h>   // for NULL

#include "state.h"
#include "tile.h"

void state_set_turn (State *state, 
		guint n_turn)
{
	assert (state != NULL);
	state->n_turn = n_turn;
}

guint state_get_turn (State *state)
{
	assert (state != NULL);
	return state->n_turn;
}

