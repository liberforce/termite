#include <stdlib.h>   // for NULL
#include <assert.h>   // for assert

#include "tile.h"

// ascii codes for tile_dump
#define TILE_LAND                   '.'
#define TILE_UNSEEN                 '?'
#define TILE_WATER                  '%'
#define TILE_FOOD                   '*'
#define TILE_DEAD_ANT               '!'  // 1 or more dead ants
#define TILE_ANT(player)           ('a' + player)
#define TILE_ANT_ON_HILL(player)   ('A' + player)
#define TILE_HILL(player)          ('0' + player)

inline void tile_set_type (Tile *tile, 
		TileType type)
{
	assert (tile != NULL);
	tile->type = type;
}

inline TileType tile_get_type (Tile *tile)
{
	assert (tile != NULL);
	return tile->type;
}

inline gboolean tile_is_free (Tile *tile)
{
	assert (tile != NULL);
	return (tile->type != TILE_TYPE_WATER && tile->type != TILE_TYPE_ANT);
}

inline gboolean tile_is_ennemy_hill (Tile *tile)
{
	assert (tile != NULL);
	return (tile->flags & TILE_HAS_HILL && tile->with.hill.owner != 0);
}

inline guint tile_get_col (Tile *tile)
{
	assert (tile != NULL);
	return tile->col;
}

inline guint tile_get_row (Tile *tile)
{
	assert (tile != NULL);
	return tile->row;
}

inline void tile_set_flags (Tile *tile,
		guint16 flags)
{
	assert (tile != NULL);
	tile->flags = flags;
}

inline guint16 tile_get_flags (Tile *tile)
{
	assert (tile != NULL);
	return tile->flags;
}

inline void tile_set_col (Tile *tile, guint col)
{
	assert (tile != NULL);
	tile->col = col;
}

inline void tile_set_row (Tile *tile, guint row)
{
	assert (tile != NULL);
	tile->row = row;
}

// Probabilities in the range [0; 10]
gchar tile_get_random_direction (Tile *tile,
		guint8 north,
		guint8 south,
		guint8 east,
		guint8 west)
{
	assert (tile != NULL);
	assert (north <= 10);
	assert (south <= 10);
	assert (east  <= 10);
	assert (west  <= 10);
	assert ((north + south + east + west) == 10);
	static const gint odds[11] = 
	{ 
		0,               // <  0%
		RAND_MAX * 0.1,  // < 10%
		RAND_MAX * 0.2,  // < 20%
		RAND_MAX * 0.3,  // < 30%
		RAND_MAX * 0.4,  // < 40%
		RAND_MAX * 0.5,  // < 50%
		RAND_MAX * 0.6,  // < 60%
		RAND_MAX * 0.7,  // < 70%
		RAND_MAX * 0.8,  // < 80%
		RAND_MAX * 0.9,  // < 90%
		RAND_MAX,        // < 100%
	};

	south += north;
	east  += south;
	west  += east;

	gint r = rand ();

	if (r < odds[north])
		return DIR_NORTH;
	else if (r < odds[south])
		return DIR_SOUTH;
	else if (r < odds[east])
		return DIR_EAST;
	else if (r < odds[west])
		return DIR_WEST;

	return DIR_NONE;
}

gchar tile_get_ascii_type (Tile *tile)
{
	assert (tile != NULL);
	switch (tile->type)
	{
		case TILE_TYPE_LAND:
		{
			if (tile->flags & TILE_HAS_HILL)
				return TILE_HILL (tile->with.hill.owner);
			else
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

