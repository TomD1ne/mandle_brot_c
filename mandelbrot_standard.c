#include "mandelbrot_iteration.h"
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    Queue *q;
    long double *result;
    Zoom *zoom;
    uint16_t max_iterations;
} ThreadWork;

static int min(int a, int b)
{
    return a < b ? a : b;
}

void *thread_work_standard(void *threadwork)
{
    const ThreadWork tw = *(ThreadWork *)threadwork;
    Rectangle *rectangle;

    while (queue_pop_front(tw.q, (void **)&rectangle) == 0)
    {
        calculate_rect(*rectangle, tw.result, tw.zoom, tw.max_iterations);
        free(rectangle);
    }

    return NULL;
}

void calculate_mandelbrot_standard(Zoom zoom, uint16_t max_iterations, long double *result, int thread_count, int lines_per_thread)
{
    const int num_of_rects = zoom.height / lines_per_thread + (zoom.height % lines_per_thread != 0);
    Queue *q = queue_init();

    for (int r = 0; r < num_of_rects; r++)
    {
        Rectangle *rectangle = malloc(sizeof(Rectangle));
        if (rectangle != NULL)
        {
            const Point tl = (Point){0, r * lines_per_thread};
            const Point br = (Point){zoom.width - 1, min(tl.y + lines_per_thread, zoom.height)};
            *rectangle = (Rectangle){tl, br};
            queue_push_back(q, rectangle);
        }
    }

    pthread_t threads[thread_count];
    ThreadWork tw = (ThreadWork){q, result, &zoom, max_iterations};
    for (int i = 0; i < thread_count; i++)
    {
        pthread_create(&threads[i], NULL, thread_work_standard, &tw);
    }

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    queue_destroy(q);
}