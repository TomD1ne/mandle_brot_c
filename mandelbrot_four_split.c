#include "mandelbrot_iteration.h"
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

typedef struct
{
    Queue *q;
    uint16_t *result;
    Zoom *zoom;
    uint16_t max_iterations;
    int *sem;
    pthread_mutex_t *sem_mutex;
    uint16_t max_rectangle_size;
} ThreadWork;

static inline int rectangle_size(Rectangle rectangle)
{
    return (rectangle.br.x - rectangle.tl.x) * (rectangle.br.y - rectangle.tl.y);
}

static inline bool rect_is_black(Rectangle rect, uint16_t *result, Zoom *zoom, uint16_t max_iterations)
{
    for (int y = rect.tl.y; y < rect.br.y; y++)
    {
        for (int x = rect.tl.x; x < rect.br.x; x++)
        {
            if (result[x + y * zoom->width] != max_iterations)
            {
                return false;
            }
        }
    }

    return true;
}

static bool calculate_rect_border(Rectangle rect, uint16_t *result, Zoom *zoom, uint16_t max_iterations)
{
    Rectangle nw = (Rectangle){{rect.tl.x, rect.tl.y}, {rect.br.x, rect.tl.y + 1}};
    Rectangle ne = (Rectangle){{rect.tl.x, rect.tl.y}, {rect.tl.x + 1, rect.br.y}};
    Rectangle sw = (Rectangle){{rect.tl.x, rect.br.y}, {rect.br.x, rect.br.y + 1}};
    Rectangle se = (Rectangle){{rect.br.x, rect.tl.y}, {rect.br.x + 1, rect.br.y}};

    calculate_rect_with_period_check(nw, result, zoom, max_iterations);
    calculate_rect_with_period_check(nw, result, zoom, max_iterations);
    calculate_rect_with_period_check(ne, result, zoom, max_iterations);
    calculate_rect_with_period_check(sw, result, zoom, max_iterations);
    calculate_rect_with_period_check(se, result, zoom, max_iterations);

    bool nw_black = rect_is_black(nw, result, zoom, max_iterations);
    bool ne_black = rect_is_black(ne, result, zoom, max_iterations);
    bool sw_black = rect_is_black(sw, result, zoom, max_iterations);
    bool se_black = rect_is_black(se, result, zoom, max_iterations);

    return nw_black && ne_black && sw_black && se_black;
}

static void split_rect(Queue *q, Rectangle rect2)
{
    Rectangle rect = (Rectangle){{rect2.tl.x + 1, rect2.tl.y + 1}, {rect2.br.x, rect2.br.y}};
    Point mid = (Point){(rect.tl.x + (rect.br.x - rect.tl.x) / 2), (rect.tl.y + (rect.br.y - rect.tl.y) / 2)};

    Rectangle *nw = malloc(sizeof(Rectangle));
    Rectangle *ne = malloc(sizeof(Rectangle));
    Rectangle *sw = malloc(sizeof(Rectangle));
    Rectangle *se = malloc(sizeof(Rectangle));

    *nw = (Rectangle){{rect.tl.x, rect.tl.y}, {mid.x, mid.y}};
    *ne = (Rectangle){{mid.x, rect.tl.y}, {rect.br.x, mid.y}};
    *sw = (Rectangle){{rect.tl.x, mid.y}, {mid.x, rect.br.y}};
    *se = (Rectangle){{mid.x, mid.y}, {rect.br.x, rect.br.y}};

    queue_push_back(q, nw);
    queue_push_back(q, ne);
    queue_push_back(q, sw);
    queue_push_back(q, se);
}

static inline void fill_black_rect(Rectangle rect, uint16_t *result, Zoom *zoom, uint16_t max_iterations)
{
    for (int y = rect.tl.y; y < rect.br.y; y++)
    {
        for (int x = rect.tl.x; x < rect.br.x; x++)
        {
            result[x + y * zoom->width] = max_iterations;
        }
    }
}

void *thread_work_four_split(void *threadwork)
{
    ThreadWork tw = *(ThreadWork *)threadwork;
    Rectangle *rectangle;

    while (1)
    {
        pthread_mutex_lock(tw.sem_mutex);
        if (queue_pop_front(tw.q, (void **)&rectangle) != 0)
        {
            if (*tw.sem == 0)
            {
                pthread_mutex_unlock(tw.sem_mutex);
                return NULL;
            }
            pthread_mutex_unlock(tw.sem_mutex);
            nanosleep(&(struct timespec){0, 100000}, NULL);
            continue;
        }

        *tw.sem = *tw.sem + 1;
        pthread_mutex_unlock(tw.sem_mutex);

        if (rectangle_size(*rectangle) > tw.max_rectangle_size)
        {
            if (calculate_rect_border(*rectangle, tw.result, tw.zoom, tw.max_iterations))
            {
                fill_black_rect(*rectangle, tw.result, tw.zoom, tw.max_iterations);
            }
            else
            {
                split_rect(tw.q, *rectangle);
            }
        }
        else
        {
            calculate_rect_with_period_check(*rectangle, tw.result, tw.zoom, tw.max_iterations);
        }
        free(rectangle);
        pthread_mutex_lock(tw.sem_mutex);
        *tw.sem = *tw.sem - 1;
        pthread_mutex_unlock(tw.sem_mutex);
    }

    return NULL;
}

void calculate_mandelbrot_four_split(Zoom zoom, uint16_t max_iterations, uint16_t *result, uint16_t thread_count, uint16_t square_size)
{
    Queue *q = queue_init();

    pthread_mutex_t sem_mutex;
    int *sem = malloc(sizeof(int));
    *sem = 0;
    pthread_mutex_init(&sem_mutex, NULL);

    Rectangle init = (Rectangle){{-1, -1}, {zoom.width + 1, zoom.height + 1}};
    split_rect(q, init);

    pthread_t threads[thread_count];
    ThreadWork tw = (ThreadWork){q, result, &zoom, max_iterations, sem, &sem_mutex, square_size};
    for (int i = 0; i < thread_count; i++)
    {
        pthread_create(&threads[i], NULL, thread_work_four_split, (void *)&tw);
    }

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    queue_destroy(q);
    free(sem);
}

void calculate_mandelbrot_four_split_benchmark(Zoom zoom, uint16_t max_iterations, uint16_t *result, uint16_t thread_count)
{
    printf("\n");
    float best_time = 9999999;
    int best_square_size = 0;

    for (uint16_t square_size = 0; square_size <= 1024; square_size += 1)
    {
        clock_t start = clock();
        calculate_mandelbrot_four_split(zoom, max_iterations, result, thread_count, square_size);
        clock_t end = clock();

        float seconds = (float)(end - start) / CLOCKS_PER_SEC;
        if (seconds < best_time)
        {
            best_time = seconds;
            best_square_size = square_size;
        }
        printf("rendered in %0.3f seconds with square size %i\n", seconds, square_size);
    }

    printf("Best rendering time: %0.3f with square_size %i\n\n", best_time, best_square_size);
}