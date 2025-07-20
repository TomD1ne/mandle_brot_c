#include "mandelbrot_types.h"

#ifndef MANDELBROT_STANDARD_H
#define MANDELBROT_STANDARD_H

void calculate_mandelbrot_standard(Zoom zoom, uint16_t max_iterations, uint16_t *result, int num_threads, int lines_per_thread);

#endif