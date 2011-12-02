#ifndef TERMITE_H
#define TERMITE_H

#include "rules.h"
#include "state.h"
#include "tile.h"

void termite_init (Rules *rules,
		State *state);

void termite_cleanup_map (Rules *rules,
		State *state);

void termite_update_state (Rules *rules, 
		State *state);

void termite_init_turn (Rules *rules,
		State *state);

void termite_play_turn (Rules *rules,
		State *state);

Tile * termite_move_ant (Rules *rules,
		State* state, 
		Tile* tile, 
		gchar dir);

gchar termite_explore (Rules *rules,
		State *state,
		Tile *tile);

gboolean termite_process_command (Rules *rules,
		State *state,
		gchar *line);

#endif // TERMITE_H
