#include "mandelbrot.h"
#include "mandelbrot_standard.h"
#include "mandelbrot_four_split.h"
#include "mandelbrot_types.h"
#include <stdlib.h>

void calculate_mandelbrot(Zoom zoom, uint16_t max_iterations, float *result, enum MandlebrotStrategy strategy, int thread_count)
{
    switch (strategy)
    {
    case STANDARD:
        calculate_mandelbrot_standard(zoom, max_iterations, result, thread_count, 1);
        break;
    case FOUR_SPLIT:
        calculate_mandelbrot_four_split(zoom, max_iterations, result, thread_count, 111);
        break;
    default:
        break;
    }
}