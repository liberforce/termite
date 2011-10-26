#ifndef TERMITE_H
#define TERMITE_H

#include "game.h"
#include "ant.h"

// Possible directions
#define DIR_NORTH 'N'
#define DIR_EAST  'E'
#define DIR_SOUTH 'S'
#define DIR_WEST  'W'
#define DIR_NONE  'X'

void termite_init_map (char *data, 
		struct game_info *game_info);

void termite_init_game (struct game_info *game_info, 
		struct game_state *game_state);

void termite_init_ants (char *data, 
		struct game_info *game_info);

void termite_do_turn (struct game_state *Game, 
		struct game_info *Info);

void termite_move_ant (struct game_state* Game, 
		struct game_info* Info, 
		Ant* ant, 
		char dir);

char termite_choose_ant_direction (struct game_state *Game, 
		struct game_info *Info, 
		Ant *ant);

#endif // TERMITE_H
