#ifndef TERMITE_H
#define TERMITE_H

#include "ants.h"
#include "ant.h"

void termite_init_map(char *data, struct game_info *game_info);
void termite_init_game (struct game_info *game_info, struct game_state *game_state);
void termite_init_ants(char *data, struct game_info *game_info);

void termite_move_ant (struct game_state* Game, struct game_info* Info, Ant* ant, char dir);
void termite_do_turn (struct game_state *Game, struct game_info *Info);

#endif // TERMITE_H
