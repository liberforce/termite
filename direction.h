#ifndef DIRECTION_H
#define DIRECTION_H

typedef enum direction_index
{
	DI_FIRST = 0,
	DI_NORTH = 0,
	DI_SOUTH,
	DI_EAST,
	DI_WEST,
	DI_LAST,
} DirectionIndex;


// Possible directions
typedef enum direction
{
	DIR_NORTH = 'N',
	DIR_EAST  = 'E',
	DIR_SOUTH = 'S',
	DIR_WEST  = 'W',
	DIR_NONE  = 'X',
} Direction;

#endif // DIRECTION_H

