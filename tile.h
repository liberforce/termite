#ifndef TILE_H
#define TILE_H

#include "types.h"
#include "ant.h"
#include "hill.h"

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

#define TILE_LAND                   '.'
#define TILE_UNSEEN                 '?'
#define TILE_WATER                  '%'
#define TILE_FOOD                   '*'
#define TILE_DEAD_ANT               '!'  // 1 or more dead ants
#define TILE_ANT(player)           ('a' + player)
#define TILE_ANT_ON_HILL(player)   ('A' + player)
#define TILE_HILL(player)          ('0' + player)

void tile_set_type (Tile *tile, 
		TileType type);

TileType tile_get_type (Tile *tile);
gchar tile_get_ascii_type (Tile *tile);
guint tile_get_col (Tile *tile);
guint tile_get_row (Tile *tile);
void tile_set_col (Tile *tile, guint col);
void tile_set_row (Tile *tile, guint row);

#endif // TILE_H

