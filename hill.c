#include <assert.h>   // for assert
#include <stdlib.h>   // for NULL

#include "hill.h"

inline guint hill_get_owner (Hill *hill)
{
	assert (hill != NULL);
	return hill->owner;
}

inline void hill_set_owner (Hill *hill, guint owner)
{
	assert (hill != NULL);
	assert (owner < 26); // owner is in the [0;25] range
	hill->owner = owner;
}

