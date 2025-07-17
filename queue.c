#include "queue.h"
#include <pthread.h>
#include <stdbool.h>

Queue *queue()
{
    Queue *q = malloc(sizeof(Queue));
    q->head = NULL;
    q->tail = NULL;
    pthread_mutex_lock(q->head_lock);
    pthread_mutex_lock(q->tail_lock);
}

// Returns a nullptr if the queue is empty, else returns a QueueItem*
void *pop_front(Queue *q)
{
    if (pthread_mutex_lock(q->head_lock) == 0)
    {
        if (q->head == NULL)
        {
            pthread_mutex_unlock(q->head_lock);
            return NULL;
        }
        else
        {
            QueueItem *itemptr = (QueueItem *)q->head;
            QueueItem item = *itemptr;
            bool is_last_item = itemptr == q->tail;

            q->head = item.prev;
            q->tail = is_last_item ? NULL : item.prev;

            QueueItem prev_item = *(QueueItem *)item.prev;
            prev_item.next = NULL;
            q->head = item.prev;
            if (prev_item.prev == NULL)
            {
                q->tail = NULL;
            }

            void *data = item.data;
            free(itemptr);

            pthread_mutex_unlock(q->head_lock);
            return data;
        }
    }
    else
    {
        printf("An error occured while locking the queue head\n");
        return NULL;
    }
}

void push_back(Queue *q, void *data)
{
}

void destroy(Queue *q)
{
}
