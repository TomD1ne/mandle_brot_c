#include "mandelbrot.h"
#include "mandelbrot_standard.h"
#include "mandelbrot_four_split.h"
#include "mandelbrot_types.h"
#include <stdlib.h>

void calculate_mandelbrot(Zoom zoom, uint16_t max_iterations, uint16_t *result, enum MandlebrotStrategy strategy, int num_threads)
{
    switch (strategy)
    {
    case STANDARD:
        calculate_mandelbrot_standard(zoom, max_iterations, result, num_threads, 1);
        break;
    case FOUR_SPLIT:
        calculate_mandelbrot_four_split(zoom, max_iterations, result, num_threads);
        break;
    default:
        break;
    }
}