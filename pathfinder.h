#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "types.h"
#include "map.h"
#include "tile.h"

typedef struct pathfinder PathFinder;

PathFinder *pathfinder_new (void);

void pathfinder_set_map (PathFinder *pf, Map *map);

gchar pathfinder_get_closest_direction (PathFinder* pf,
		Tile *tile,
		Tile *target);

gchar pathfinder_get_most_attractive_direction (PathFinder* pf,
		Tile *tile);

void pathfinder_propagate_attractivity (PathFinder *pf,
		Tile *tile,
		gint attractivity,
		gint8 depth);

#endif // PATHFINDER_H
