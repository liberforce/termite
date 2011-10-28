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

Map *map_new (guint n_rows, guint n_cols, gchar filler); 
void map_free (Map *map);
gchar * map_get_buffer (Map *map);
guint map_get_length (Map *map);
guint map_get_n_rows (Map *map);
guint map_get_n_cols (Map *map);
gchar map_get_content (Map *map, gint row, gint col);
void map_get_cardinals (Map *map,
		gint row,
		gint col,
		struct cardinals *seen);

#ifndef NDEBUG
void map_dump (Map *map);
#else
#define map_dump(...)
#endif

#endif // MAP_H
