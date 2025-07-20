#include "mandelbrot_iteration.h"
#include "queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct
{
    Queue *q;
    int *result;
    Zoom *zoom;
    int max_iterations;
} ThreadWork;

int min(int a, int b)
{
    return (a < b) ? a : b;
}

void *thread_work(void *threadwork)
{
    ThreadWork tw = *(ThreadWork *)threadwork;
    Rectangle *rectangle;

    while (queue_pop_front(tw.q, (void **)&rectangle) == 0)
    {
        calculate_rect(*rectangle, tw.result, tw.zoom, tw.max_iterations);
        free(rectangle);
    }

    return NULL;
}

void calculate_mandlebrot_standard(Zoom zoom, int max_iterations, int *result, int num_threads, int lines_per_thread)
{
    int num_of_lines = zoom.height / lines_per_thread + (zoom.height % lines_per_thread != 0);
    Queue *q = queue_init();

    for (int c = 0; c < num_of_lines; c++)
    {
        Rectangle *rectangle = malloc(sizeof(Rectangle));
        if (rectangle != NULL)
        {
            Point tl = (Point){0, c * num_of_lines};
            Point br = (Point){zoom.width - 1, min(tl.y + num_of_lines, zoom.height)};
            *rectangle = (Rectangle){tl, br};
            queue_push_back(q, rectangle);
        }
    }

    pthread_t threads[num_threads];
    ThreadWork tw = (ThreadWork){q, result, &zoom, max_iterations};
    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&threads[i], NULL, thread_work, (void *)&tw);
    }

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    queue_destroy(q);
}