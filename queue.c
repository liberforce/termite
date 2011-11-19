#include <stdlib.h>   // for calloc, malloc
#include <assert.h>   // for assert

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

inline void queue_reset (Queue *queue)
{
	assert (queue_is_empty (queue));

	// Rewind to avoid overflow
	queue->start = queue->end = queue->data;
}

inline gboolean queue_is_empty (Queue *queue)
{
	assert (queue != NULL);
	assert (queue->start <= queue->end);
	return queue->start == queue->end;
}
