#include "queue.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

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
    if (pthread_mutex_lock(&q->lock) != 0)
    {
        printf("An error occured while locking the queue head\n");
        return -1;
    }

    if (q->head == NULL)
    {
        pthread_mutex_unlock(&q->lock);
        return -1;
    }

    QueueItem *item = (QueueItem *)q->head;
    *result = item->data;
    q->head = item->prev;

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

    if (pthread_mutex_lock(&q->lock) != 0)
    {
        printf("An error occured while locking the queue tail\n");
        return;
    }

    QueueItem *item = malloc(sizeof(QueueItem));
    item->data = data;
    item->prev = NULL;

    if (q->tail == NULL)
    {
        q->tail = item;
        q->head = item;
    }
    else
    {
        q->tail->prev = item;
        q->tail = item;
    }

    pthread_mutex_unlock(&q->lock);
}

void queue_destroy(Queue *q)
{
    while (q->head != NULL)
    {
        QueueItem *item = q->head;
        q->head = item->prev;
        free(item);
    }
    pthread_mutex_destroy(&q->lock);
    free(q);
}
