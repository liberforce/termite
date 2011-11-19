#ifndef QUEUE_H
#define QUEUE_H

#include "types.h"

typedef struct queue Queue;

Queue * queue_new (guint n_elements);

void queue_push (Queue *queue, gpointer data);

gpointer queue_pop (Queue *queue);

Queue * queue_reset (Queue *queue);

gboolean queue_is_empty (Queue *queue);

guint queue_get_n_elements (Queue *queue);

void queue_push_queue (Queue *queue, Queue *other);

#endif // QUEUE_H
