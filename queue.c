#include "queue.h"
#include <pthread.h>
#include <stdlib.h>

Queue *queue_init()
{
    Queue *q = malloc(sizeof(Queue));
    if (!q)
        return NULL;

    q->head = NULL;
    q->tail = NULL;
    pthread_mutex_init(&q->lock, NULL);

    return q;
}

int queue_pop_front(Queue *q, void **result)
{
    pthread_mutex_lock(&q->lock);

    if (q->head == NULL)
    {
        pthread_mutex_unlock(&q->lock);
        return -1;
    }

    QueueItem *item = q->head;
    *result = item->data;
    q->head = item->next;

    if (q->tail == item)
    {
        q->tail = NULL;
    }

    free(item);
    pthread_mutex_unlock(&q->lock);
    return 0;
}

void queue_push_back(Queue *q, void *data)
{
    pthread_mutex_lock(&q->lock);
    QueueItem *item = malloc(sizeof(QueueItem));
    item->data = data;
    item->next = NULL;

    if (q->tail == NULL)
    {
        q->tail = item;
        q->head = item;
    }
    else
    {
        q->tail->next = item;
        q->tail = item;
    }

    pthread_mutex_unlock(&q->lock);
}

void queue_destroy(Queue *q)
{
    while (q->head != NULL)
    {
        QueueItem *item = q->head;
        q->head = item->next;
        free(item);
    }
    pthread_mutex_destroy(&q->lock);
    free(q);
}
