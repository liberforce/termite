#ifndef ANT_H
#define ANT_H

typedef struct ant 
{
	int id;
	int row;
	int col;
	int owner;
} Ant;

int ant_get_col (Ant *ant);
int ant_get_row (Ant *ant);
int ant_get_owner (Ant *ant);

void ant_set_col (Ant *ant, int col);
void ant_set_row (Ant *ant, int row);
void ant_set_owner (Ant *ant, int player_id);

#endif // ANT_H
