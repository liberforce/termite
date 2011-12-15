#include <assert.h>     // for assert
#include <stdlib.h>     // for calloc
#include <string.h>     // for memmove

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
	Queue *tileset;
};

inline PathFinder *pathfinder_new (void)
{
	PathFinder *pf = calloc (1, sizeof(PathFinder));
	pf->queue1 = queue_new (1024);
	pf->queue2 = queue_new (1024);
	pf->tileset = queue_new (1024);
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

		// Don't propagate on unexplored tiles, we may propagate in water
		if (! tile_is_flag_set (t, TILE_FLAG_IS_EXPLORED))
			continue;

		Cardinals look;
		map_get_cardinals (pf->map,
				tile_get_row (t),
				tile_get_col (t),
				look);

		DirectionIndex di;
		for (di = DI_FIRST; di < DI_LAST; ++di)
		{
			Tile *other = look[di];
			if (! (tile_is_flag_set (other, TILE_FLAG_BEING_PROCESSED)
					|| tile_is_flag_set (other, TILE_FLAG_IS_WATER)))
			{
				queue_push (output, other);
				tile_set_flag (other, TILE_FLAG_BEING_PROCESSED);
				tile_set_attractivity (other, max (tile_get_attractivity (other), attractivity));
			}
		}
	}
}

void pathfinder_propagate_attractivity (PathFinder *pf,
		Tile *tile,
		gint attractivity,
		gint step,
		gint8 depth)
{
	assert (pf != NULL);
	assert (tile != NULL);
	assert (depth >= 0);

	Queue *input  = queue_reset (pf->queue1);
	Queue *output = queue_reset (pf->queue2);
	Queue *tileset = queue_reset (pf->tileset);

	// Feed the input to start finding neigbours
	queue_push (input, tile);

	// The target tile needs to be part of the final tileset 
	// so the TILE_FLAG_BEING_PROCESSED flag can be removed
	queue_push (output, tile);

	// We set attractivity level when an input tile is consumed
	tile_set_flag (tile, TILE_FLAG_BEING_PROCESSED);
	tile_set_attractivity (tile, max (attractivity, tile_get_attractivity (tile)));

	while (! queue_is_empty (input) && --depth >= 0)
	{
		attractivity += step;
		pathfinder_select_tile_group_set_attractivity (pf,
				input,
				output,
				attractivity);

		// Avoids overflow
		queue_reset (input);

		// Memorize output
		queue_push_queue (tileset, output);

		// To get neighbours for next depth level, reuse by swapping input and
		// output queues
		Queue *tmp = input;
		input = output;
		output = tmp;
	}

	while (! queue_is_empty (tileset))
	{
		Tile *t = queue_pop (tileset);
		tile_unset_flag (t, TILE_FLAG_BEING_PROCESSED);
	}
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

static void pathfinder_select_if_in_range (PathFinder *pf,
		Queue *queue,
		Tile *tile,
		Tile *other,
		guint distance_sq)
{
	if (tile_is_flag_set (other, TILE_FLAG_BEING_PROCESSED))
		return;

	guint d = map_distance_sq (pf->map, tile, other);

	if (d < distance_sq)
	{
		queue_push (queue, other);
		tile_set_flag (other, TILE_FLAG_BEING_PROCESSED);

		Cardinals look;
		map_get_cardinals (pf->map,
				tile_get_row (other),
				tile_get_col (other),
				look);

		DirectionIndex di;
		for (di = DI_FIRST; di < DI_LAST; di++)
		{
			pathfinder_select_if_in_range (pf,
					queue,
					tile,
					look[di],
					distance_sq);
		}
	}
}

Queue *pathfinder_select_in_range_sq (PathFinder *pf,
		Tile *tile,
		guint distance_sq)
{
	assert (pf != NULL);
	assert (tile != NULL);

	queue_reset (pf->tileset);	
	pathfinder_select_if_in_range (pf,
			pf->tileset,
			tile,
			tile,
			distance_sq);
	return pf->tileset;
}

// Returns a valid direction at random.
gchar pathfinder_get_random_direction (PathFinder *pf,
		Tile *tile)
{
	assert (pf != NULL);
	assert (tile != NULL);

	gchar dir = DIR_NONE;
	guint8 n_valid_moves = 0;
	gchar valid_moves[DI_LAST];
	Cardinals look = { NULL, NULL, NULL, NULL };
	map_get_cardinals (pf->map, tile_get_row (tile), tile_get_col (tile), look);

	DirectionIndex di;
	for (di = DI_FIRST; di < DI_LAST; di++)
	{
		Tile *t = look[di];

		if (! tile_is_flag_set (t, TILE_FLAG_IS_WATER) 
				&& ! tile_is_flag_set (t, TILE_FLAG_HAS_ANT))
		{
			valid_moves[n_valid_moves++] = direction_from_index (di);
		}
	}

	if (n_valid_moves > 0)
	{
		guint r = rand ();
		dir = valid_moves[r % n_valid_moves];
	}

	return dir;
}

gchar pathfinder_explore_least_seen (PathFinder *pf,
		Tile *tile)
{
	assert (tile != NULL);
	assert (tile_is_flag_set (tile, TILE_FLAG_HAS_ANT));
	assert (ant_get_owner (&tile->with.ant) == 0);

	gchar dir = DIR_NONE;
	gchar last_dir = ant_get_direction (&tile->with.ant);
	DirectionIndex last_dir_index = direction_to_index (last_dir);
	Cardinals look = { NULL, NULL, NULL, NULL };
	map_get_cardinals (pf->map, tile_get_row (tile), tile_get_col (tile), look);

	guint n_seen = G_MAX_UINT;
	DirectionIndex di;
	for (di = DI_FIRST; di < DI_LAST; di++)
	{
		Tile *t = look[di];

		if (tile_is_flag_set (t, TILE_FLAG_IS_WATER) 
				|| tile_is_flag_set (t, TILE_FLAG_HAS_ANT))
			continue;

		guint s = tile_get_seen (t);
		if (s < n_seen || (s == n_seen && last_dir_index == di))
		{
			n_seen = s;
			dir = direction_from_index (di);
		}
	}

	return dir;
}
