#include "mandelbrot_types.h"

#ifndef MANDELBROT_FOUR_SPLIT_H
#define MANDELBROT_FOUR_SPLIT_H

void calculate_mandelbrot_four_split(Zoom zoom, uint16_t max_iterations, uint16_t *result, int num_threads);

#endif