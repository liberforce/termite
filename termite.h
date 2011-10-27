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
		char *data);

void termite_init_map (char *data, 
		State *state);

void termite_init_game (Rules *rules, 
		State *state);

void termite_do_turn (State *state, 
		Rules *rules);

void termite_move_ant (State* state, 
		Rules *rules, 
		Ant* ant, 
		char dir);

char termite_choose_ant_direction (State *state, 
		Rules *rules, 
		Ant *ant);

#endif // TERMITE_H
