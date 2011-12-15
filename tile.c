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

inline gboolean tile_is_flag_set (Tile *tile,
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

inline void tile_incr_seen (Tile *tile)
{
	assert (tile != NULL);
	++tile->seen;
}

inline void tile_decr_seen (Tile *tile)
{
	assert (tile != NULL);
	--tile->seen;
}

inline guint tile_get_seen (Tile *tile)
{
	assert (tile != NULL);
	return tile->seen;
}

inline gboolean tile_is_seen (Tile *tile)
{
	assert (tile != NULL);
	return (tile->seen > 0);
}

inline void tile_set_seen (Tile *tile,
		guint seen)
{
	assert (tile != NULL);
	tile->seen = seen;
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

	if (! tile_is_seen (tile))
		return TILE_UNSEEN;

	if (tile_is_flag_set (tile, TILE_FLAG_HAS_DEAD_ANT))
		return TILE_DEAD_ANT;

	if (! tile_is_flag_set (tile, TILE_FLAG_IS_WATER))
		return TILE_LAND;

	// Should never be reached
	assert (0);
	return '#';
}

