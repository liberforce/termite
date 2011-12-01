#include <assert.h>

#include "direction.h"

gchar direction_from_index (DirectionIndex di)
{
	assert (di >= DI_FIRST);
	assert (di < DI_LAST);
	static const gchar dir[DI_LAST] = 
	{ 
		DIR_NORTH,
		DIR_SOUTH,
		DIR_EAST,
		DIR_WEST
	};
	return dir[di];
}

DirectionIndex direction_to_index (gchar dir)
{
	switch (dir)
	{
		case 'N': return DI_NORTH;
		case 'S': return DI_SOUTH;
		case 'E': return DI_EAST;
		case 'W': return DI_WEST;
		default:
			  assert (0);
			  return DI_FIRST;
	};
}

