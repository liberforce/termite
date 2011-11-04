#include <assert.h>     // for assert
#include <stdlib.h>     // for calloc

#include "pathfinder.h"
#include "utils.h"
#include "debug.h"

struct pathfinder
{
	Map *map;
};

inline PathFinder *pathfinder_new (void)
{
	return calloc (1, sizeof(PathFinder));
}

inline void pathfinder_set_map (PathFinder *pf, Map *map)
{
	assert (pf != NULL);
	assert (map != NULL);
	pf->map = map;
}

// Needs to be done on map, as tiles know nothing about map wrapping
inline gchar pathfinder_get_closest_direction (PathFinder* pf,
		Tile *tile,
		Tile *target)
{
	assert (pf != NULL);
	assert (pf->map != NULL);
	assert (tile != NULL);
	assert (target != NULL);

	gchar dir = DIR_NONE;
	struct cardinals look = { NULL };
	map_get_cardinals (pf->map, tile_get_row (tile), tile_get_col (tile), &look);

	gint dr = wrapped_vector (tile_get_row (tile), tile_get_row (target), map_get_n_rows (pf->map));
	gint dc = wrapped_vector (tile_get_col (tile), tile_get_col (target), map_get_n_cols (pf->map));

	g_debug ("delta row = %d, delta col = %d\n", dr, dc);

	if (tile_is_free (look.north)
	    	&& dr < 0)
		dir = DIR_NORTH;
	else if (tile_is_free (look.south)
	    	&& dr > 0)
		dir = DIR_SOUTH;
	else if (tile_is_free (look.east)
	    	&& dc > 0)
		dir = DIR_EAST;
	else if (tile_is_free (look.west)
	    	&& dc < 0)
		dir = DIR_WEST;

	// DIR_NONE may happen if we can't move or if tile == target
	return dir;
}

