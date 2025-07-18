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

void calculate_chunk(Rectangle *rectangle, int *result, Zoom *zoom, int max_iterations)
{
    for (int x = rectangle->tl.x; x < rectangle->br.x; x++)
    {
        for (int y = rectangle->tl.y; y < rectangle->br.y; y++)
        {
            Complex c = calculate_complex(x, y, zoom);
            result[x + y * zoom->width] = mandelbrot_iterations(c, max_iterations);
        }
    }
}

void *calculate_chunks(void *threadwork)
{
    ThreadWork *tw = (ThreadWork *)threadwork;
    Rectangle *rectangle;

    while (queue_pop_front(tw->q, (void **)&rectangle) == 0)
    {
        calculate_chunk(rectangle, tw->result, tw->zoom, tw->max_iterations);
        free(rectangle);
    }
    return tw;
}

void calculate_mandlebrot_standard(Zoom zoom, int max_iterations, int *result, int num_threads, int chunck_size)
{
    int squares_in_width = zoom.width / chunck_size + (zoom.width % chunck_size != 0);
    int squares_in_height = zoom.height / chunck_size + (zoom.height % chunck_size != 0);
    Queue *q = queue_init();

    for (int y = 0; y < squares_in_width; y++)
    {
        for (int x = 0; x < squares_in_height; x++)
        {
            Rectangle *rectangle = malloc(sizeof(Rectangle));
            if (rectangle != NULL)
            {
                Point tl = (Point){chunck_size * x, chunck_size * y};
                Point br = (Point){min(tl.x + chunck_size, zoom.width), min(tl.y + chunck_size, zoom.height)};
                *rectangle = (Rectangle){tl, br};
                queue_push_back(q, rectangle);
            }
        }
    }

    pthread_t threads[num_threads];
    ThreadWork tw = (ThreadWork){q, result, &zoom, max_iterations};
    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&threads[i], NULL, calculate_chunks, (void *)&tw);
    }

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    queue_destroy(q);
}