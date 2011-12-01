#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "types.h"
#include "map.h"
#include "tile.h"
#include "queue.h"

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
		gint step,
		gint8 depth);

Queue *pathfinder_select_in_range_sq (PathFinder *pf,
		Tile *tile,
		guint distance_sq);

gchar pathfinder_explore_random (PathFinder *pf,
		Tile *tile);

gchar pathfinder_explore_least_seen (PathFinder *pf,
		Tile *tile);

#endif // PATHFINDER_H
