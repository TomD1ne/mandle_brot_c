#pragma once
#include "mandelbrot_types.h"
#include <stdint.h>

#ifndef MANDELBROT_FOUR_SPLIT_H
#define MANDELBROT_FOUR_SPLIT_H

void calculate_mandelbrot_four_split(Zoom zoom, uint16_t max_iterations, double *result, uint16_t thread_count, uint16_t square_size);
void calculate_mandelbrot_four_split_benchmark(Zoom zoom, uint16_t max_iterations, double *result, uint16_t thread_count);

#endif