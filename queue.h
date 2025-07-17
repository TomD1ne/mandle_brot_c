#ifndef QUEUE_H
#define QUEUE_H

typedef struct
{
    QueueItem *head;
    QueueItem *tail;
    pthread_mutex_t *head_lock;
    pthread_mutex_t *tail_lock;
} Queue;

typedef struct
{
    QueueItem *prev;
    QueueItem *next;
    void *data;
} QueueItem;

Queue queue();
void *pop_front(Queue *q);
void push_back(Queue *q, void *data);
void destroy(Queue *q);

#endif