#include "mandelbrot_types.h"

#ifndef MANDELBROT_STANDARD_H
#define MANDELBROT_STANDARD_H

void calculate_mandlebrot_standard(Zoom zoom, int max_iterations, int *result, int num_threads, int lines_per_thread);

#endif