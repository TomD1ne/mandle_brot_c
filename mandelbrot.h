#include "mandelbrot_types.h"

#ifndef MANDELBROT_H
#define MANDELBROT_H

enum MandlebrotStrategy
{
    STANDARD,   // calculate each point, pixel per pixel
    FOUR_SPLIT, // iteratively split into four planes
};

void calculate_mandelbrot(Zoom z, int max_iterations, int *result, enum MandlebrotStrategy strategy, int num_threads);

#endif