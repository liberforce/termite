#include <stdlib.h>   // for NULL
#include <assert.h>   // for assert

#include "tile.h"

// ascii codes for tile_dump
#define TILE_LAND                   '.'
#define TILE_UNEXPLORED             '#'
#define TILE_UNSEEN                 '?'
#define TILE_WATER                  '%'
#define TILE_FOOD                   '*'
#define TILE_DEAD_ANT               '!'  // 1 or more dead ants
#define TILE_ANT(player)           ('a' + player)
#define TILE_ANT_ON_HILL(player)   ('A' + player)
#define TILE_HILL(player)          ('0' + player)

inline gboolean tile_is_free (Tile *tile)
{
	assert (tile != NULL);
	return ((tile->flags & (TILE_FLAG_IS_WATER | TILE_FLAG_HAS_ANT | TILE_FLAG_HAS_FOOD)) == 0);
}

inline gboolean tile_has_enemy_hill (Tile *tile)
{
	assert (tile != NULL);
	return (tile->flags & TILE_FLAG_HAS_HILL && tile->with.hill.owner != 0);
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
		TileFlags flags)
{
	assert (tile != NULL);
	tile->flags = flags;
}

inline TileFlags tile_get_flags (Tile *tile)
{
	assert (tile != NULL);
	return tile->flags;
}

inline void tile_set_flag (Tile *tile,
		TileFlags flag)
{
	assert (tile != NULL);
	tile->flags |= flag;
}

inline void tile_unset_flag (Tile *tile,
		TileFlags flag)
{
	assert (tile != NULL);
	tile->flags &= ~flag;
}

inline TileFlags tile_is_flag_set (Tile *tile,
		TileFlags flag)
{
	assert (tile != NULL);
	return tile->flags & flag;
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

inline void tile_add_attractivity (Tile *tile,
		gint step)
{
	assert (tile != NULL);
	tile->attractivity += step;
}

inline void tile_set_attractivity (Tile *tile,
		gint attractivity)
{
	assert (tile != NULL);
	tile->attractivity = attractivity;
}

inline gint tile_get_attractivity (Tile *tile)
{
	assert (tile != NULL);
	return tile->attractivity;
}

// Probabilities in the range [0; 100]
gchar tile_get_random_direction (Tile *tile,
		guint8 north,
		guint8 south,
		guint8 east,
		guint8 west)
{
	assert (tile != NULL);
	assert (north <= 100);
	assert (south <= 100);
	assert (east  <= 100);
	assert (west  <= 100);
	assert ((north + south + east + west) == 100);
	static const guint odds[101] = 
	{ 
		0,
		RAND_MAX * 0.01,
		RAND_MAX * 0.02,
		RAND_MAX * 0.03,
		RAND_MAX * 0.04,
		RAND_MAX * 0.05,
		RAND_MAX * 0.06,
		RAND_MAX * 0.07,
		RAND_MAX * 0.08,
		RAND_MAX * 0.09,
		RAND_MAX * 0.10,
		RAND_MAX * 0.11,
		RAND_MAX * 0.12,
		RAND_MAX * 0.13,
		RAND_MAX * 0.14,
		RAND_MAX * 0.15,
		RAND_MAX * 0.16,
		RAND_MAX * 0.17,
		RAND_MAX * 0.18,
		RAND_MAX * 0.19,
		RAND_MAX * 0.20,
		RAND_MAX * 0.21, 
		RAND_MAX * 0.22, 
		RAND_MAX * 0.23, 
		RAND_MAX * 0.24, 
		RAND_MAX * 0.25, 
		RAND_MAX * 0.26, 
		RAND_MAX * 0.27, 
		RAND_MAX * 0.28, 
		RAND_MAX * 0.29, 
		RAND_MAX * 0.30, 
		RAND_MAX * 0.31, 
		RAND_MAX * 0.32, 
		RAND_MAX * 0.33, 
		RAND_MAX * 0.34, 
		RAND_MAX * 0.35, 
		RAND_MAX * 0.36, 
		RAND_MAX * 0.37, 
		RAND_MAX * 0.38, 
		RAND_MAX * 0.39, 
		RAND_MAX * 0.40, 
		RAND_MAX * 0.41, 
		RAND_MAX * 0.42, 
		RAND_MAX * 0.43, 
		RAND_MAX * 0.44, 
		RAND_MAX * 0.45, 
		RAND_MAX * 0.46, 
		RAND_MAX * 0.47, 
		RAND_MAX * 0.48, 
		RAND_MAX * 0.49, 
		RAND_MAX * 0.50, 
		RAND_MAX * 0.51, 
		RAND_MAX * 0.52, 
		RAND_MAX * 0.53, 
		RAND_MAX * 0.54, 
		RAND_MAX * 0.55, 
		RAND_MAX * 0.56, 
		RAND_MAX * 0.57, 
		RAND_MAX * 0.58, 
		RAND_MAX * 0.59, 
		RAND_MAX * 0.60, 
		RAND_MAX * 0.61, 
		RAND_MAX * 0.62, 
		RAND_MAX * 0.63, 
		RAND_MAX * 0.64, 
		RAND_MAX * 0.65, 
		RAND_MAX * 0.66, 
		RAND_MAX * 0.67, 
		RAND_MAX * 0.68, 
		RAND_MAX * 0.69, 
		RAND_MAX * 0.70, 
		RAND_MAX * 0.71, 
		RAND_MAX * 0.72, 
		RAND_MAX * 0.73, 
		RAND_MAX * 0.74, 
		RAND_MAX * 0.75, 
		RAND_MAX * 0.76, 
		RAND_MAX * 0.77, 
		RAND_MAX * 0.78, 
		RAND_MAX * 0.79, 
		RAND_MAX * 0.80, 
		RAND_MAX * 0.81, 
		RAND_MAX * 0.82, 
		RAND_MAX * 0.83, 
		RAND_MAX * 0.84, 
		RAND_MAX * 0.85, 
		RAND_MAX * 0.86, 
		RAND_MAX * 0.87, 
		RAND_MAX * 0.88, 
		RAND_MAX * 0.89, 
		RAND_MAX * 0.90, 
		RAND_MAX * 0.91, 
		RAND_MAX * 0.92, 
		RAND_MAX * 0.93, 
		RAND_MAX * 0.94, 
		RAND_MAX * 0.95, 
		RAND_MAX * 0.96, 
		RAND_MAX * 0.97, 
		RAND_MAX * 0.98, 
		RAND_MAX * 0.99, 
		RAND_MAX,       
	};

	south += north;
	east  += south;
	west  += east;

	guint r = rand ();

	// We need to handle the case when r == 0 and r == RAND_MAX
	// West uses inferior or equal to avoid RAND_MAX not being handled
	if (r < odds[north])
		return DIR_NORTH;
	else if (r < odds[south])
		return DIR_SOUTH;
	else if (r < odds[east])
		return DIR_EAST;
	else if (r <= odds[west])
		return DIR_WEST;

	// We should never get here
	assert (0);
	return DIR_NONE;
}

gchar tile_get_ascii_type (Tile *tile)
{
	assert (tile != NULL);
	if (tile_is_flag_set (tile, TILE_FLAG_IS_WATER))
		return TILE_WATER;

	if (tile_is_flag_set (tile, TILE_FLAG_HAS_FOOD))
		return TILE_FOOD;
	
	if (tile_is_flag_set (tile, TILE_FLAG_HAS_ANT))
	{
		if (tile_is_flag_set (tile, TILE_FLAG_HAS_HILL))
			return TILE_ANT_ON_HILL (tile->with.ant.owner);
		else
			return TILE_ANT (tile->with.ant.owner);

	}
	
	if (tile_is_flag_set (tile, TILE_FLAG_HAS_HILL))
		return TILE_HILL (tile->with.hill.owner);

	if (! tile_is_flag_set (tile, TILE_FLAG_IS_EXPLORED))
		return TILE_UNEXPLORED;

	if (!tile_is_flag_set (tile, TILE_FLAG_IS_SEEN))
		return TILE_UNSEEN;

	if (tile_is_flag_set (tile, TILE_FLAG_HAS_DEAD_ANT))
		return TILE_DEAD_ANT;

	if (! tile_is_flag_set (tile, TILE_FLAG_IS_WATER))
		return TILE_LAND;

	// Should never be reached
	assert (0);
	return '#';
}

