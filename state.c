#include <assert.h>
#include <stdlib.h>   // for NULL

#include "state.h"
#include "time.h"     // for g_get_time

inline void state_set_turn (State *state, 
		guint n_turn)
{
	assert (state != NULL);
	state->n_turn = n_turn;
}

inline guint state_get_turn (State *state)
{
	assert (state != NULL);
	return state->n_turn;
}

inline void state_timer_reset (State *state)
{
	assert (state != NULL);
	state->timer = g_get_time ();
}

inline gint64 state_timer_get_elapsed (State *state)
{
	assert (state != NULL);
	return (g_get_time () - state->timer);
}
