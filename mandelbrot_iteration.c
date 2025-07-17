#include "mandelbrot_iteration.h"
#include "mandelbrot_types.h"

int mandelbrot_iterations(Complex c, int max_iterations)
{
    Complex z = {0.0, 0.0};
    int iterations = 0;
    while (z.real * z.real + z.imag * z.imag <= 4.0 && iterations < max_iterations)
    {
        double temp = z.real * z.real - z.imag * z.imag + c.real;
        z.imag = 2.0 * z.real * z.imag + c.imag;
        z.real = temp;
        iterations++;
    }

    return iterations;
}

Complex calculate_complex(int x, int y, int width, int height, Zoom z)
{
    Complex c;
    c.real = (x - width / 2.0) / z.factor + z.offset_x;
    c.imag = (y - height / 2.0) / z.factor + z.offset_y;
    return c;
}