#include <stdio.h>     // for NULL
#include <assert.h>    // for assert
#include <stdlib.h>    // for malloc, calloc
#include <string.h>    // for memset
#include "map.h"

struct map
{
	gchar *data;
	guint n_rows;
	guint n_cols;
	guint length;
};

inline Map *map_new (guint n_rows, guint n_cols, gchar filler)
{
	Map *map = calloc (1, sizeof (Map));
	assert (map != NULL);
	map->n_rows = n_rows;
	map->n_cols = n_cols;
	map->length = n_cols * n_rows;
	map->data = malloc (map->length * sizeof (gchar));
	memset (map->data, filler, map->length);
	return map;
}

inline void map_free (Map *map)
{
	assert (map != NULL);
	free (map->data);
	free (map);
}

inline gchar * map_get_buffer (Map *map)
{
	assert (map != NULL);
	return map->data;
}

inline guint map_get_length (Map *map)
{
	assert (map != NULL);
	return map->length;
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

inline void map_set_tile (Map *map, gint row, gint col, gchar tile)
{
	assert (map != NULL);
	assert (map->data != NULL);
	assert (row < map->n_rows);
	assert (col < map->n_cols);
	map->data[row * map->n_cols + col] = tile;
}

inline gchar map_get_tile (Map *map, gint row, gint col)
{
	assert (map != NULL);
	assert (map->data != NULL);
	assert (row < map->n_rows);
	assert (col < map->n_cols);
	return map->data[row * map->n_cols + col];
}

void map_get_cardinals (Map *map,
		gint row,
		gint col,
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

	seen->north = map->data[offset + up   ];
	seen->south = map->data[offset + down ];
	seen->east  = map->data[offset + right];
	seen->west  = map->data[offset + left ];
}

#ifndef NDEBUG
inline void map_dump (Map *map)
{
	assert (map != NULL);
	gint i, j;
	for (i = 0; i < map->n_rows; i++)
	{
		for (j = 0; j < map->n_cols; j++)
		{
			fprintf (stderr, "%c ", map_get_tile (map, i, j));
		}
		fprintf (stderr, "\n");
	}
	fprintf (stderr, "\n");
	fflush (stderr);
}
#endif

