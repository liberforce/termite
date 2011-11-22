#ifndef HILL_H
#define HILL_H

#include "types.h"

typedef struct hill
{
	guint8 owner;
} Hill;

guint8 hill_get_owner (Hill *hill);

void hill_set_owner (Hill *hill, guint8 owner);

#endif // HILL_H
