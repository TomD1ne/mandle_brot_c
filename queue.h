#include <pthread.h>

#ifndef QUEUE_H
#define QUEUE_H

typedef struct QueueItem
{
    void *data;
    struct QueueItem *next;
} QueueItem;

typedef struct
{
    pthread_mutex_t lock;
    QueueItem *head;
    QueueItem *tail;
} Queue;

Queue *queue_init();
int queue_pop_front(Queue *q, void **result);
void queue_push_back(Queue *q, void *data);
void queue_destroy(Queue *q);

#endif