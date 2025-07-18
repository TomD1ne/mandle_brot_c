#include "mandelbrot.h"
#include "mandelbrot_standard.h"
#include "mandelbrot_types.h"

void calculate_mandlebrot_four_split(Zoom zoom, int max_iterations, int *result, int num_threads)
{
}

void calculate_mandelbrot(Zoom zoom, int max_iterations, int *result, enum MandlebrotStrategy strategy, int num_threads)
{
    switch (strategy)
    {
    case STANDARD:
        calculate_mandlebrot_standard(zoom, max_iterations, result, num_threads, 16);
        break;
    case FOUR_SPLIT:
        calculate_mandlebrot_four_split(zoom, max_iterations, result, num_threads);
        break;
    default:
        break;
    }
}