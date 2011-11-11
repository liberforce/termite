#ifndef TILE_H
#define TILE_H

#include "types.h"
#include "ant.h"
#include "hill.h"

// Possible directions
#define DIR_NORTH 'N'
#define DIR_EAST  'E'
#define DIR_SOUTH 'S'
#define DIR_WEST  'W'
#define DIR_NONE  'X'

typedef enum
{
	TILE_FLAG_IS_WATER        = 0x01,
	TILE_FLAG_IS_SEEN         = 0x02,
	TILE_FLAG_IS_EXPLORED     = 0x04,
	TILE_FLAG_HAS_ANT         = 0x08,
	TILE_FLAG_HAS_HILL        = 0x10,
	TILE_FLAG_HAS_FOOD        = 0x20,
	TILE_FLAG_HAS_DEAD_ANT    = 0x40,
	TILE_FLAG_BEING_PROCESSED = 0x80,
} TileFlags;

typedef struct tile
{
	TileFlags flags;
	guint row;
	guint col;
	guint attractivity;
	union 
	{
		Ant ant;
		Hill hill;
	} with; 
} Tile;

gboolean tile_is_free (Tile *tile);
gboolean tile_has_enemy_hill (Tile *tile);
void tile_set_flags (Tile *tile, TileFlags flags);
TileFlags tile_get_flags (Tile *tile);
void tile_set_flag (Tile *tile,
		TileFlags flag);
void tile_unset_flag (Tile *tile,
		TileFlags flag);
gboolean tile_is_flag_set (Tile *tile,
		TileFlags flag);
gchar tile_get_ascii_type (Tile *tile);
guint tile_get_col (Tile *tile);
guint tile_get_row (Tile *tile);
void tile_set_col (Tile *tile, guint col);
void tile_set_row (Tile *tile, guint row);
gchar tile_get_random_direction (Tile *tile,
		guint8 north,
		guint8 south,
		guint8 east,
		guint8 west);

void tile_add_attractivity (Tile *tile,
		gint step);

void tile_set_attractivity (Tile *tile,
		gint attractivity);

gint tile_get_attractivity (Tile *tile);

#endif // TILE_H

