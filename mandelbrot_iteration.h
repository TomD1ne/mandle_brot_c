#pragma once
#include "mandelbrot_types.h"
#include <stdint.h>

#ifndef MANDELBROT_ITERATION_H
#define MANDELBROT_ITERATION_H

double mandelbrot_iterations(Complex c, uint16_t max_iterations);
Complex calculate_complex(int x, int y, const Zoom *z);
void calculate_rect(Rectangle rect, double *result, const Zoom *zoom, uint16_t max_iterations);
void calculate_rect_with_period_check(Rectangle rect, double *result, const Zoom *zoom, uint16_t max_iterations);

#endif