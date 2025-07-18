#include "mandelbrot_types.h"

#ifndef MANDELBROT_STANDARD_H
#define MANDELBROT_STANDARD_H

void calculate_mandlebrot_standard(Zoom z, int max_iterations, int *result, int num_threads, int chunk_size);

#endif