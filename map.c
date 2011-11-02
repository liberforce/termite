#include <stdio.h>     // for NULL
#include <assert.h>    // for assert
#include <stdlib.h>    // for malloc, calloc
#include <string.h>    // for memset

#include "map.h"
#include "utils.h"
#include "debug.h"

struct map
{
	Tile *tiles;
	guint n_rows;
	guint n_cols;
	guint n_elements;
};

inline Map *map_new (guint n_rows, guint n_cols, TileType filler)
{
	Map *map = calloc (1, sizeof (Map));
	guint i, j;
	assert (map != NULL);
	map->n_rows = n_rows;
	map->n_cols = n_cols;
	map->n_elements = n_cols * n_rows;
	map->tiles = malloc (map->n_elements * sizeof (Tile));

	for (i = 0; i < n_rows; i++)
	{
		for (j = 0; j < n_cols; j++)
		{
			Tile *t = &map->tiles[i * n_cols + j];
			t->type = filler;
			t->row = i;
			t->col = j;
		}
	}

	return map;
}

inline void map_free (Map *map)
{
	assert (map != NULL);
	free (map->tiles);
	free (map);
}

inline Tile * map_get_buffer (Map *map)
{
	assert (map != NULL);
	return map->tiles;
}

inline guint map_get_n_elements (Map *map)
{
	assert (map != NULL);
	return map->n_elements;
}

inline guint map_get_n_rows (Map *map)
{
	assert (map != NULL);
	return map->n_rows;
}

inline guint map_get_n_cols (Map *map)
{
	assert (map != NULL);
	return map->n_cols;
}

inline Tile * map_get_tile (Map *map, guint row, guint col)
{
	assert (map != NULL);
	assert (map->tiles != NULL);
	assert (row < map->n_rows);
	assert (col < map->n_cols);
	return &map->tiles[row * map->n_cols + col];
}

void map_get_cardinals (Map *map,
		guint row,
		guint col,
		struct cardinals *seen)
{
	assert (map != NULL);
	assert (seen != NULL);

	gint offset = row * map->n_cols + col;
	gint down = map->n_cols;
	gint up = -down; 
	gint left = -1;
	gint right = 1;

	// Now here is the tricky part. We have to account for
	// the fact that the map wraps (when you go off one edge
	// you end up on the side of the map opposite that edge).
	// This is done by checking to see if we are on the last
	// row or column and if the direction we are taking would
	// take us off the side of the map.
	//
	// For example, you can see here the West direction checks
	// to see if we are in the first column, in which case "West"
	// is a character a full row minus one from our location.

	if (row == 0 
			|| row == (map->n_rows - 1)
			|| col == 0
			|| col == (map->n_cols - 1))
	{
		if (row == 0)
			up = (map->n_rows - 1) * map->n_cols;
		else if (row == map->n_rows - 1)
			down = - (map->n_rows - 1) * map->n_cols;

		if (col == 0)
			left = map->n_cols - 1;
		else if (col == map->n_cols - 1)
			right = - (map->n_cols -1);
	}

	seen->north = &map->tiles[offset + up   ];
	seen->south = &map->tiles[offset + down ];
	seen->east  = &map->tiles[offset + right];
	seen->west  = &map->tiles[offset + left ];
}

inline gboolean map_tile_in_range (Map *map,
		Tile *tile, 
		Tile *other,
		guint max_range_sq)
{
	return map_distance_sq (map,
			tile,
			other) <= max_range_sq;
}

inline guint map_distance_sq (Map *map,
		Tile *tile, 
		Tile *other)
{
	assert (map != NULL);
	assert (tile != NULL);
	assert (other != NULL);

	return distance_sq (tile_get_row (tile),
			tile_get_col (tile),
			tile_get_row (other),
			tile_get_col (other),
			map_get_n_rows (map),
			map_get_n_cols (map));
}

#ifndef NDEBUG
inline void map_dump (Map *map)
{
	assert (map != NULL);
	gint i, j;

	g_debug ("\n");
	for (i = 0; i < map->n_rows; i++)
	{
		for (j = 0; j < map->n_cols; j++)
		{
			Tile *tile = map_get_tile (map, i, j);
			fprintf (stderr, "%c", tile_get_ascii_type (tile));
		}
		fprintf (stderr, "\n");
	}
	g_debug ("\n");
}
#endif

