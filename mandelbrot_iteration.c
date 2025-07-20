#include "mandelbrot_iteration.h"

uint16_t mandelbrot_iterations(Complex c, uint16_t max_iterations)
{
    Complex z = {0.0, 0.0};
    uint16_t iterations = 0;
    while (z.real * z.real + z.imag * z.imag <= 4.0 && iterations < max_iterations)
    {
        double temp = z.real * z.real - z.imag * z.imag + c.real;
        z.imag = 2.0 * z.real * z.imag + c.imag;
        z.real = temp;
        iterations++;
    }

    return iterations;
}

Complex calculate_complex(int x, int y, Zoom *zoom)
{
    Complex c;
    c.real = (x - zoom->width / 2.0) / zoom->factor + zoom->offset_x;
    c.imag = (y - zoom->height / 2.0) / zoom->factor + zoom->offset_y;
    return c;
}

void calculate_rect(Rectangle rect, uint16_t *result, Zoom *zoom, uint16_t max_iterations)
{
    for (int x = rect.tl.x; x < rect.br.x; x++)
    {
        for (int y = rect.tl.y; y < rect.br.y; y++)
        {
            Complex c = calculate_complex(x, y, zoom);
            result[x + y * zoom->width] = mandelbrot_iterations(c, max_iterations);
        }
    }
}