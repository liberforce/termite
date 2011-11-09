#ifndef QUEUE_H
#define QUEUE_H

#include "types.h"

typedef struct queue Queue;

Queue * queue_new (guint n_elements);

void queue_push (Queue *queue, gpointer data);

gpointer queue_pop (Queue *queue);

gboolean queue_is_empty (Queue *queue);

#endif // QUEUE_H
