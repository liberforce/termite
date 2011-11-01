#ifndef MAP_H
#define MAP_H

#include "types.h"
#include "tile.h"

struct cardinals
{
	TileType north;
	TileType south;
	TileType east;
	TileType west;
};

typedef struct map Map;

Map *map_new (guint n_rows, 
		guint n_cols, 
		TileType filler);
void map_free (Map *map);
Tile * map_get_buffer (Map *map);
guint map_get_n_elements (Map *map);
guint map_get_n_rows (Map *map);
guint map_get_n_cols (Map *map);
Tile * map_get_tile (Map *map, 
		guint row, 
		guint col);
void map_get_cardinals (Map *map,
		guint row,
		guint col,
		struct cardinals *seen);

gboolean map_is_in_range (Map *map,
		Tile *tile, 
		Tile *other,
		guint max_range_sq);

#ifndef NDEBUG
void map_dump (Map *map);
#else
#define map_dump(...)
#endif

#endif // MAP_H
