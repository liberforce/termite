#ifndef HILL_H
#define HILL_H

#include "types.h"

typedef struct hill
{
	guint owner;
} Hill;

guint hill_get_owner (Hill *hill);

void hill_set_owner (Hill *hill, guint owner);

#endif // HILL_H
