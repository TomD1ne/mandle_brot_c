#include "mandelbrot.h"
#include "mandelbrot_standard.h"
#include "mandelbrot_four_split.h"
#include "mandelbrot_types.h"
#include <stdint.h>

void calculate_mandelbrot(const Zoom zoom, const uint16_t max_iterations, long double *result, const enum MandelbrotStrategy strategy, const int thread_count)
{
    switch (strategy)
    {
    case STANDARD:
        calculate_mandelbrot_standard(zoom, max_iterations, result, thread_count, 100);
        break;
    case FOUR_SPLIT:
        calculate_mandelbrot_four_split(zoom, max_iterations, result, thread_count, 110);
        break;
    default:
        break;
    }
}