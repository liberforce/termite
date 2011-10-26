#include <stdio.h>     // for NULL
#include <assert.h>    // for assert
#include <stdlib.h>    // for malloc, calloc
#include "map.h"

struct map
{
	gchar *data;
	guint n_rows;
	guint n_cols;
};

inline Map *map_new (guint n_rows, guint n_cols)
{
	Map *map = calloc (1, sizeof (Map));
	assert (map = NULL);
	map->n_rows = n_rows;
	map->n_cols = n_cols;
	map->data = calloc (1, n_rows * n_cols);
	return map;
}

inline void map_free (Map *map)
{
	assert (map != NULL);
	free (map->data);
	free (map);
}

