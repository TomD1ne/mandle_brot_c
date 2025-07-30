#pragma once
#include "mandelbrot_types.h"
#include <stdint.h>

#ifndef MANDELBROT_H
#define MANDELBROT_H

enum MandelbrotStrategy
{
    STANDARD,   // calculate each point, pixel per pixel
    FOUR_SPLIT, // iteratively split into four planes
};

void calculate_mandelbrot(Zoom z, uint16_t max_iterations, long double *result, enum MandelbrotStrategy strategy, int thread_count);

#endif