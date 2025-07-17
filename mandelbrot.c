#include "mandelbrot.h"
#include "mandelbrot_standard.h"
#include "mandelbrot_types.h"

void calculate_mandelbrot(int width, int height, Zoom z, int max_iterations, int *result, enum MandlebrotStrategy strategy, int num_threads)
{
    switch (strategy)
    {
    case STANDARD:
        calculate_mandlebrot_standard(width, height, z, max_iterations, result, num_threads, 4);
        break;
    case FOUR_SPLIT:
        calculate_mandlebrot_four_split(width, height, z, max_iterations, result, num_threads);
        break;
    default:
        break;
    }
}

void calculate_mandlebrot_four_split(int width, int height, Zoom z, int max_iterations, int *result, int num_threads)
{
}