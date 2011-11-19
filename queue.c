#include <stdlib.h>   // for calloc, malloc
#include <assert.h>   // for assert
#include <string.h>   // for memmove

#include "queue.h"

struct queue
{
	gpointer *data;
	gpointer *start;
	gpointer *end;
	guint len;
};

inline Queue * queue_new (guint n_elements)
{
	Queue *queue = calloc (1, sizeof (Queue));
	assert (queue != NULL);
	queue->len = 1024;
	queue->data = malloc (queue->len * sizeof (gpointer));
	queue->start = queue->data;
	queue->end = queue->data;
	return queue;
}

inline void queue_push (Queue *queue, gpointer data)
{
	assert (queue != NULL);
	assert (queue->end < queue->start + queue->len);
	*queue->end++ = data;
}

inline gpointer queue_pop (Queue *queue)
{
	assert (queue != NULL);
	assert (queue->start < queue->end);
	gpointer data = *queue->start;
	queue->start++;
	return data;
}

inline Queue* queue_reset (Queue *queue)
{
	assert (queue != NULL);

	// Rewind to avoid overflow
	queue->start = queue->end = queue->data;
	return queue;
}

inline guint queue_get_n_elements (Queue *queue)
{
	assert (queue != NULL);

	return (queue->end - queue->start) / sizeof (gpointer);
}

inline void queue_push_queue (Queue *queue, Queue *other)
{
	assert (queue != NULL);
	assert (other != NULL);

	guint n =  queue_get_n_elements (other);
	memmove (queue->end, other->start, n * sizeof (gpointer));
	queue->end += n;
}

inline gboolean queue_is_empty (Queue *queue)
{
	assert (queue != NULL);
	assert (queue->start <= queue->end);
	return queue->start == queue->end;
}
