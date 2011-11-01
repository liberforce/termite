#include <stdlib.h>   // for NULL
#include <assert.h>   // for assert

#include "tile.h"

void tile_set_type (Tile *tile, 
		TileType type)
{
	assert (tile != NULL);
	tile->type = type;
}

TileType tile_get_type (Tile *tile)
{
	assert (tile != NULL);
	return tile->type;
}

gchar tile_get_ascii_type (Tile *tile)
{
	assert (tile != NULL);
	switch (tile->type)
	{
		case TILE_TYPE_LAND:
		{
			// TODO: handle hill
			// if (tile->flags & TILE_HAS_HILL)
			//	return TILE_HILL (tile->with.hill.owner);
			// else
				return TILE_LAND;
		}

		case TILE_TYPE_UNSEEN:
			return TILE_UNSEEN;

		case TILE_TYPE_WATER:
			return TILE_WATER;

		case TILE_TYPE_FOOD:
			return TILE_FOOD;

		case TILE_TYPE_DEAD_ANT:
			return TILE_DEAD_ANT;

		case TILE_TYPE_ANT:
		{
			if (tile->flags & TILE_HAS_HILL)
				return TILE_ANT_ON_HILL (tile->with.ant.owner);
			else
				return TILE_ANT (tile->with.ant.owner);
		}

		default:
			assert (0);
			return '#';
	}
}

