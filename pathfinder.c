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

gchar pathfinder_explore_random (PathFinder *pf,
		Tile *tile)
{
	assert (tile != NULL);
	assert (tile_is_flag_set (tile, TILE_FLAG_HAS_ANT));
	assert (ant_get_owner (&tile->with.ant) == 0);

	Map *map = pf->map;
	gboolean n, s, e, w;
	guint8 n_free_tiles = 0;
	gchar last_dir = ant_get_direction (&tile->with.ant);
	Cardinals look = { NULL, NULL, NULL, NULL };

	map_get_cardinals (map, tile_get_row (tile), tile_get_col (tile), look);

	n = tile_is_free (look[DI_NORTH]);
	s = tile_is_free (look[DI_SOUTH]);
	e = tile_is_free (look[DI_EAST]);
	w = tile_is_free (look[DI_WEST]);
	
	n_free_tiles = n + s + e + w;

	// Pick a direction at random to avoid ants stay blocked on hill
	if (last_dir != DIR_NORTH
			&& last_dir != DIR_SOUTH
			&& last_dir != DIR_EAST
			&& last_dir != DIR_WEST)
		last_dir =  tile_get_random_direction (tile, 25, 25, 25, 25);

	if (n_free_tiles == 4)
	{
		if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 70, 10, 10, 10);
		if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 10, 70, 10, 10);
		if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 10, 10, 70, 10);
		if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 10, 10, 10, 70);
		assert (0);
	}
	else if (n_free_tiles == 3)
	{
		if (n && s && e)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 70, 10, 20, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 10, 70, 20, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 10, 10, 80, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 40, 40, 20, 0);
			assert (0);
		}
		else if (n && s && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 70, 10, 0, 20);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 10, 70, 0, 20);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 40, 40, 0, 20);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 10, 10, 0, 80);
			assert (0);
		}
		else if (s && e && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 20, 40, 40);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 80, 10, 10);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 20, 70, 10);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 20, 10, 70);
			assert (0);
		}
		else if (n && e && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 80, 0, 10, 10);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 20, 0, 40, 40);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 20, 0, 70, 10);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 20, 0, 10, 70);
			assert (0);
		}
	}
	else if (n_free_tiles == 2)
	{
		if (n && s)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 90, 10, 0, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 10, 90, 0, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 50, 50, 0, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 50, 50, 0, 0);
			assert (0);
		}
		else if (e && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 0, 50, 50);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 0, 50, 50);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 0, 90, 10);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 0, 10, 90);
			assert (0);
		}
		else if (e && s)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 10, 90, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 80, 20, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 20, 80, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 90, 10, 0);
			assert (0);
		}
		else if (s && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 10, 0, 90);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 80, 0, 20);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 90, 0, 10);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 20, 0, 80);
			assert (0);
		}
		else if (n && e)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 80, 0, 20, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 10, 0, 90, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 20, 0, 80, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 90, 0, 10, 0);
			assert (0);
		}
		else if (s && e)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 0, 10, 90, 0);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 0, 80, 20, 0);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 0, 20, 80, 0);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 0, 90, 10, 0);
			assert (0);
		}
		else if (n && w)
		{
			if (last_dir == DIR_NORTH) return tile_get_random_direction (tile, 80, 0, 0, 20);
			if (last_dir == DIR_SOUTH) return tile_get_random_direction (tile, 10, 0, 0, 90);
			if (last_dir == DIR_EAST)  return tile_get_random_direction (tile, 90, 0, 0, 10);
			if (last_dir == DIR_WEST)  return tile_get_random_direction (tile, 20, 0, 0, 80);
			assert (0);
		}
	}
	else if (n_free_tiles == 1)
	{
		if (n) return DIR_NORTH;
		else if (s) return DIR_SOUTH;
		else if (e) return DIR_EAST;
		else if (w) return DIR_WEST;
		assert (0);
	}

	// Unlikely but may happen when surrounded by ants 
	if (n_free_tiles == 0)
		return DIR_NONE;

	// We should never get here!
	assert (0);
	return DIR_NONE;
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

		if (tile_is_flag_set (t, TILE_FLAG_IS_WATER))
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
