#ifndef RULES_H
#define RULES_H

#include "types.h"

typedef struct rules 
{
	guint loadtime;
	guint turntime;
	guint rows;
	guint cols;
	guint turns;
	guint viewradius_sq;
	guint attackradius_sq;
	guint spawnradius_sq;
	gint64 seed;
} Rules;

#endif // RULES_H

