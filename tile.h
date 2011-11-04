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
	TILE_TYPE_LAND = 0,
	TILE_TYPE_UNSEEN,
	TILE_TYPE_WATER,
	TILE_TYPE_FOOD,
	TILE_TYPE_DEAD_ANT,
	TILE_TYPE_ANT,
} TileType;

typedef struct tile
{
	guint16 type;
	guint16 flags;
	guint row;
	guint col;
	union 
	{
		Ant ant;
		Hill hill;
	} with; 

} Tile;

#define TILE_HAS_HILL              (1 << 0)

void tile_set_type (Tile *tile, 
		TileType type);

TileType tile_get_type (Tile *tile);
gboolean tile_is_free (Tile *tile);
gboolean tile_is_ennemy_hill (Tile *tile);
void tile_set_flags (Tile *tile, guint16 flags);
guint16 tile_get_flags (Tile *tile);
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

#endif // TILE_H

