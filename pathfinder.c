#include <assert.h>     // for assert
#include <stdlib.h>     // for calloc

#include "pathfinder.h"
#include "queue.h"
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

	gchar dir = DIR_NONE;
	struct cardinals look = { NULL, NULL, NULL, NULL };
	map_get_cardinals (pf->map, tile_get_row (tile), tile_get_col (tile), &look);

	gint dr = wrapped_vector (tile_get_row (tile), tile_get_row (target), map_get_n_rows (pf->map));
	gint dc = wrapped_vector (tile_get_col (tile), tile_get_col (target), map_get_n_cols (pf->map));

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

void pathfinder_propagate_attractivity (PathFinder *pf,
		Tile *tile,
		gint attractivity)
{
	assert (pf != NULL);
	assert (tile != NULL);
	assert (abs (attractivity) < map_get_n_rows (pf->map));
	assert (abs (attractivity) < map_get_n_cols (pf->map));

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

	while (! queue_is_empty (output) && attractivity != 0)
	{
		while (! queue_is_empty (input))
		{
			Tile *t = queue_pop (input);
			tile_unset_flag (t, TILE_FLAG_BEING_PROCESSED);

			struct cardinals look = { NULL, NULL, NULL, NULL };
			map_get_cardinals (pf->map,
					tile_get_row (t),
					tile_get_col (t),
					&look);

			if (! tile_is_flag_set (look.north, TILE_FLAG_BEING_PROCESSED))
			{
				queue_push (output, look.north);
				tile_set_flag (look.north, TILE_FLAG_BEING_PROCESSED);
				tile_add_attractivity (look.north, attractivity);
			}

			if (! tile_is_flag_set (look.south, TILE_FLAG_BEING_PROCESSED))
			{
				queue_push (output, look.south);
				tile_add_attractivity (look.south, attractivity);
				tile_set_flag (look.south, TILE_FLAG_BEING_PROCESSED);
			}

			if (! tile_is_flag_set (look.east, TILE_FLAG_BEING_PROCESSED))
			{
				queue_push (output, look.east);
				tile_add_attractivity (look.east, attractivity);
				tile_set_flag (look.east, TILE_FLAG_BEING_PROCESSED);
			}

			if (! tile_is_flag_set (look.west, TILE_FLAG_BEING_PROCESSED))
			{
				queue_push (output, look.west);
				tile_add_attractivity (look.west, attractivity);
				tile_set_flag (look.west, TILE_FLAG_BEING_PROCESSED);
			}
		}

		--attractivity;

		// Avoids overflow
		queue_reset (input);

		// Get neighbours from next level -> swap input and output queues
		Queue *tmp = input;
		input = output;
		output = tmp;
	}
}

