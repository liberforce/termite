#include "ants.h"
#include "MyBot.h"
#include "ant.h"

// The following is an example program displaying a basic ai that
// tries all possible directions and then moves in whichever one
// is not blocked by water (% characters).
//
// To see all the information contained in struct game_state and
// struct game_info check ants.h. There is also a distance function
// provided in bot.c

void bot_do_turn(struct game_state *Game, struct game_info *Info) {

	// defining things just so we can do less writing
	// UP and DOWN move up and down rows while LEFT and RIGHT
	// move side to side. The map is just one big array.

#define UP -Info->cols
#define DOWN Info->cols
#define LEFT -1
#define RIGHT 1

	int i;

	for (i = 0; i < Game->my_count; ++i) 
	{
		// the location within the map array where our ant is currently
		Ant *ant = &Game->my_ants[i];

		int offset = ant_get_row (ant) * Info->cols + ant_get_col (ant);

		// Now here is the tricky part. We have to account for
		// the fact that the map wraps (when you go off one edge
		// you end up on the side of the map opposite that edge).
		// This is done by checking to see if we are on the last
		// row or column and if the direction we are taking would
		// take us off the side of the map.
		//
		// For example, you can see here the West direction checks
		// to see if we are in the first column, in which case "West"
		// is a character a full row minus one from our location.

		char obj_north, obj_east, obj_south, obj_west;

		if (ant_get_col (ant) != 0)
			obj_west = Info->map[offset + LEFT];
		else
			obj_west = Info->map[offset + Info->cols - 1];

		if (ant_get_col (ant) != Info->cols - 1)
			obj_east = Info->map[offset + RIGHT];
		else
			obj_east = Info->map[offset - Info->cols - 1];

		if (ant_get_row (ant) != 0)
			obj_north = Info->map[offset + UP];
		else
			obj_north = Info->map[offset + (Info->rows - 1)*Info->cols];

		if (ant_get_row (ant) != Info->rows - 1)
			obj_south = Info->map[offset + DOWN];
		else
			obj_south = Info->map[offset - (Info->rows - 1)*Info->cols];

		char dir = -1;

		// cycle through the directions, pick one that works

		if (obj_north != '%')
			dir = 'N';
		else if (obj_east != '%')
			dir = 'E';
		else if (obj_south != '%')
			dir = 'S';
		else if (obj_west != '%')
			dir = 'W';

		// Now we do our move

		if (dir != -1)
			bot_move_ant (Game, Info, ant, dir);
	}

	// There are many ways to make this program better.
	// For starters, try to avoid collisions between your
	// own ants and make a conscious effort to gather food
	// instead of walking around at random.
	//
	// Good luck!
}
