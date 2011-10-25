#ifndef ANT_H
#define ANT_H

typedef struct ant 
{
	int id;
	int row;
	int col;
} Ant;

int ant_get_col (Ant *ant);
int ant_get_row (Ant *ant);
void ant_set_col (Ant *ant, int col);
void ant_set_row (Ant *ant, int row);

#endif // ANT_H
