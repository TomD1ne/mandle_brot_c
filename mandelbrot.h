#include "mandelbrot_types.h"
#include "stdlib.h"

#ifndef MANDELBROT_H
#define MANDELBROT_H

enum MandlebrotStrategy
{
    STANDARD,   // calculate each point, pixel per pixel
    FOUR_SPLIT, // iteratively split into four planes
};

void calculate_mandelbrot(Zoom z, uint16_t max_iterations, uint16_t *result, enum MandlebrotStrategy strategy, int num_threads);

#endif