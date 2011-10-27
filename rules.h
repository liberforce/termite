#ifndef RULES_H
#define RULES_H

typedef struct rules 
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
} Rules;

#endif // RULES_H

