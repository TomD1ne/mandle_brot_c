#pragma once
#include "mandelbrot_types.h"
#include <stdint.h>

#ifndef MANDELBROT_STANDARD_H
#define MANDELBROT_STANDARD_H

void calculate_mandelbrot_standard(Zoom zoom, uint16_t max_iterations, long double *result, int thread_count, int lines_per_thread);

#endif