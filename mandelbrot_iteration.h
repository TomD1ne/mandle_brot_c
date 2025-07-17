#ifndef MANDELBROT_ITERATION_H
#define MANDELBROT_ITERATION_H

int mandelbrot_iterations(Complex c, int max_iterations);
Complex calculate_complex(int x, int y, int width, int height, Zoom z);

#endif