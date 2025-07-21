#include "mandelbrot_types.h"
#include <stdlib.h>

#ifndef MANDELBROT_ITERATION_H
#define MANDELBROT_ITERATION_H

uint16_t mandelbrot_iterations(Complex c, uint16_t max_iterations);
Complex calculate_complex(int x, int y, Zoom *z);
void calculate_rect(Rectangle rect, float *result, Zoom *zoom, uint16_t max_iterations);
void calculate_rect_simd_neon_double(Rectangle rect, uint16_t *result, Zoom *zoom, uint16_t max_iterations);
void calculate_rect_with_period_check(Rectangle rect, uint16_t *result, Zoom *zoom, uint16_t max_iterations);
void calculate_rect_with_period_check_smooth(Rectangle rect, float *result, Zoom *zoom, uint16_t max_iterations);

#endif