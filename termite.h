#ifndef MYBOT_H
#define MYBOT_H

void bot_move_ant (struct game_state* Game, struct game_info* Info, Ant* ant, char dir);
void bot_do_turn (struct game_state *Game, struct game_info *Info);

#endif // MYBOT_H
