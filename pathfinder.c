#include <assert.h>     // for assert
#include <stdlib.h>     // for calloc

#include "pathfinder.h"
#include "queue.h"
#include "direction.h"
#include "utils.h"
#include "debug.h"

struct pathfinder
{
	Map *map;
	Queue *queue1;
	Queue *queue2;
};

inline PathFinder *pathfinder_new (void)
{
	PathFinder *pf = calloc (1, sizeof(PathFinder));
	pf->queue1 = queue_new (1024);
	pf->queue2 = queue_new (1024);
	return pf;
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

	Cardinals look;
	map_get_cardinals (pf->map, tile_get_row (tile), tile_get_col (tile), look);

	gint dr = wrapped_vector (tile_get_row (tile), tile_get_row (target), map_get_n_rows (pf->map));
	gint dc = wrapped_vector (tile_get_col (tile), tile_get_col (target), map_get_n_cols (pf->map));

	if (tile_is_free (look[DI_NORTH]) && dr < 0)
		return DIR_NORTH;

	if (tile_is_free (look[DI_SOUTH]) && dr > 0)
		return DIR_SOUTH;

	if (tile_is_free (look[DI_EAST]) && dc > 0)
		return DIR_EAST;

	if (tile_is_free (look[DI_WEST]) && dc < 0)
		return DIR_WEST;

	// DIR_NONE may happen if we can't move or if tile == target
	return DIR_NONE;
}

static void pathfinder_select_tile_group_set_attractivity (PathFinder *pf,
		Queue *input, 
		Queue *output,
		gint attractivity)
{
	assert (input != NULL);
	assert (output != NULL);

	while (! queue_is_empty (input))
	{
		Tile *t = queue_pop (input);

		Cardinals look;
		map_get_cardinals (pf->map,
				tile_get_row (t),
				tile_get_col (t),
				look);

		DirectionIndex di;
		for (di = DI_FIRST; di < DI_LAST; ++di)
		{
			if (! tile_is_flag_set (look[di], TILE_FLAG_BEING_PROCESSED))
			{
				queue_push (output, look[di]);
				tile_set_flag (look[di], TILE_FLAG_BEING_PROCESSED);
				tile_add_attractivity (look[di], attractivity);
			}
		}
	}
}

void pathfinder_propagate_attractivity (PathFinder *pf,
		Tile *tile,
		gint attractivity,
		gint8 depth)
{
	assert (pf != NULL);
	assert (tile != NULL);
	assert (depth >= 0);

	Queue *input  = pf->queue1;
	Queue *output = pf->queue2;

	// Feed the input to start finding neigbours
	queue_push (input, tile);

	// Add center tile to output so it gets the same attractivity bonus as
	// neighbours, because when 2 food items are in diagonal, we want to 
	// favorize positions where both can be eaten at once
	queue_push (output, tile);

	// We set attractivity level when an input tile is consumed
	tile_set_flag (tile, TILE_FLAG_BEING_PROCESSED);
	tile_add_attractivity (tile, attractivity);

	while (! queue_is_empty (input) && --depth >= 0)
	{
		pathfinder_select_tile_group_set_attractivity (pf,
				input,
				output,
				attractivity--);

		// Avoids overflow
		queue_reset (input);

		// To get neighbours for next depth level, reuse by swapping input and
		// output queues
		Queue *tmp = input;
		input = output;
		output = tmp;
	}

	queue_reset (input);
	queue_reset (output);
}


gchar pathfinder_get_most_attractive_direction (PathFinder* pf,
		Tile *tile)
{
	assert (pf != NULL);
	assert (pf->map != NULL);
	assert (tile != NULL);

	gchar dir = DIR_NONE;
	static const gchar dirs[DI_LAST] = { DIR_NORTH, DIR_SOUTH, DIR_EAST, DIR_WEST };
	Cardinals look;
	DirectionIndex di;

	map_get_cardinals (pf->map, tile_get_row (tile), tile_get_col (tile), look);

	guint att = tile_get_attractivity (tile);
	guint att2;

	for (di = DI_FIRST; di < DI_LAST; ++di)
	{
		if (tile_is_free (look[di]))
		{
			att2 = tile_get_attractivity (look[di]);
			if (att2 > att)
			{
				att = att2;
				dir = dirs[di];
			}
		}
	}

	// DIR_NONE may happen if we can't move
	return dir;
}

