#include <pthread.h>

#ifndef QUEUE_H
#define QUEUE_H

typedef struct QueueItem
{
    struct QueueItem *prev;
    struct QueueItem *next;
    void *data;
} QueueItem;

typedef struct
{
    QueueItem *head;
    QueueItem *tail;
    pthread_mutex_t lock;
} Queue;

Queue *queue_init();
int queue_pop_front(Queue *q, void **result);
void queue_push_back(Queue *q, void *data);
void queue_destroy(Queue *q);

#endif