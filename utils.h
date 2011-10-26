#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include "ants.h"

int abs (int x);
int max (int a, int b);
int min (int a, int b);
int distance (int row1, int col1, int row2, int col2, struct game_info *Info);

#endif // UTILS_H
