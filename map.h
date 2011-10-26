#ifndef MAP_H
#define MAP_H

#include "types.h"

struct cardinals
{
	gchar north;
	gchar south;
	gchar east;
	gchar west;
};

typedef struct map Map;

Map *map_new (guint n_rows, guint n_cols); 
void map_free (Map *map);

#endif
