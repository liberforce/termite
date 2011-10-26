#ifndef ANTS_H
#define ANTS_H

#include "ant.h"

// this header is basically self-documenting

struct game_info 
{
	int loadtime;
	int turntime;
	int rows;
	int cols;
	int turns;
	int viewradius_sq;
	int attackradius_sq;
	int spawnradius_sq;
	int seed;
	char *map;
};

struct food 
{
	int row;
	int col;
};

struct game_state
{
	Ant *my_ants;
	Ant *enemy_ants;
	Ant *dead_ants;

	struct food *food;

	int my_count;
	int enemy_count;
	int food_count;
	int dead_count;

	int my_ant_index;
};

#endif // ANTS_H
