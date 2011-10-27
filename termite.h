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

void termite_init_map (char *data, 
		struct game_state *state);

void termite_init_game (Rules *rules, 
		struct game_state *game_state);

void termite_init_ants (char *data, 
		Rules *rules);

void termite_do_turn (struct game_state *state, 
		Rules *rules);

void termite_move_ant (struct game_state* state, 
		Rules *rules, 
		Ant* ant, 
		char dir);

char termite_choose_ant_direction (struct game_state *Game, 
		Rules *rules, 
		Ant *ant);

#endif // TERMITE_H
