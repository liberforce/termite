#ifndef TERMITE_H
#define TERMITE_H

#include "rules.h"
#include "state.h"
#include "ant.h"

// Possible directions
#define DIR_NORTH 'N'
#define DIR_EAST  'E'
#define DIR_SOUTH 'S'
#define DIR_WEST  'W'
#define DIR_NONE  'X'

void termite_init (Rules *rules,
		State *state,
		gchar *data);

void termite_update_map (Rules *rules,
		State *state, 
		gchar *data);

void termite_update_state (Rules *rules, 
		State *state);

void termite_play_turn (Rules *rules,
		State *state);

void termite_move_ant (Rules *rules,
		State* state, 
		Ant* ant, 
		gchar dir);

gchar termite_choose_ant_direction (Rules *rules,
		State *state,
		Ant *ant);

#endif // TERMITE_H
