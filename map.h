#ifndef MAP_H
#define MAP_H

#include "types.h"

typedef struct map Map;

Map *map_new (guint n_rows, guint n_cols); 
void map_free (Map *map);

#endif
