#include "mandelbrot_types.h"

#ifndef MANDELBROT_ITERATION_H
#define MANDELBROT_ITERATION_H

int mandelbrot_iterations(Complex c, int max_iterations);
Complex calculate_complex(int x, int y, Zoom *z);
void calculate_rect(Rectangle rect, int *result, Zoom *zoom, int max_iterations);

#endif