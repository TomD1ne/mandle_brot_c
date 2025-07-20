#include "mandelbrot_iteration.h"
#include "queue.h"
#include <pthread.h>
#include <stdio.h>

typedef struct
{
    Queue *q;
    uint16_t *result;
    Zoom *zoom;
    uint16_t max_iterations;
    int *sem;
    pthread_mutex_t *sem_mutex;
} ThreadWork;

int rectangle_size(Rectangle rectangle)
{
    return (rectangle.br.x - rectangle.tl.x) * (rectangle.br.y - rectangle.tl.y);
}

void calculate_rect_border()
{
}

void split_rect()
{
}

void fill_empty_rect()
{
}

void *thread_work_four_split(void *threadwork)
{
    ThreadWork tw = *(ThreadWork *)threadwork;
    Rectangle *rectangle;
    int max_rectangle_size = 16;

    while (1)
    {
        pthread_mutex_lock(tw.sem_mutex);
        if (queue_pop_front(tw.q, (void **)&rectangle) != 0)
        {
            if (tw.sem == 0)
            {
                pthread_mutex_unlock(tw.sem_mutex);
                break;
            }
            pthread_mutex_unlock(tw.sem_mutex);
        }
        if (rectangle_size(*rectangle) > max_rectangle_size)
        {
            split_rect();
        }
        else
        {
            calculate_rect(*rectangle, tw.result, tw.zoom, tw.max_iterations);
        }
        free(rectangle);
    }

    return NULL;
}

void calculate_mandelbrot_four_split(Zoom zoom, uint16_t max_iterations, uint16_t *result, int num_threads)
{
    Queue *q = queue_init();

    pthread_mutex_t sem_mutex;
    int *sem = malloc(sizeof(int));
    *sem = 0;
    pthread_mutex_init(&sem_mutex, NULL);

    int half_width = zoom.width / 2 + zoom.width % 2;
    int half_height = zoom.height / 2 + zoom.height % 2;

    for (int x = 0; x < 2; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            Rectangle *rectangle = malloc(sizeof(Rectangle));
            if (rectangle != NULL)
            {
                Point tl = (Point){x * half_width, y * half_height};
                Point br = (Point){x == 0 ? half_width : zoom.width - 1, y == 0 ? half_height : zoom.height - 1};
                *rectangle = (Rectangle){tl, br};
                queue_push_back(q, rectangle);
            }
        }
    }

    pthread_t threads[num_threads];
    ThreadWork tw = (ThreadWork){q, result, &zoom, max_iterations, sem, &sem_mutex};
    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&threads[i], NULL, thread_work_four_split, (void *)&tw);
    }

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    queue_destroy(q);
}