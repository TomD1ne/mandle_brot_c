#include "mandelbrot_iteration.h"
#include <stdint.h>
#include <math.h>

long double mandelbrot_iterations(const Complex c, const uint16_t max_iterations)
{
    Complex z = {0.0, 0.0};
    uint16_t iterations = 0;
    while (z.real * z.real + z.imag * z.imag <= 4.0 && iterations < max_iterations)
    {
        const long double temp = z.real * z.real - z.imag * z.imag + c.real;
        z.imag = 2.0 * z.real * z.imag + c.imag;
        z.real = temp;

        if (z.real * z.real + z.imag * z.imag > 4.0)
        {
            long double log_zn = log(z.real * z.real + z.imag * z.imag) / 2.0;
            long double nu = log(log_zn / log(2.0)) / log(2.0);
            return (long double)(iterations + 1) - nu;
        }
        iterations++;
    }

    return (long double)iterations;
}

Complex calculate_complex(const int x, const int y, const Zoom *zoom)
{
    Complex c;
    c.real = (x - zoom->width / 2.0) / zoom->factor + zoom->offset_x;
    c.imag = (y - zoom->height / 2.0) / zoom->factor + zoom->offset_y;
    return c;
}

void calculate_rect(const Rectangle rect, long double *result, const Zoom *zoom, const uint16_t max_iterations)
{
    for (int y = rect.tl.y; y < rect.br.y; y++)
    {
        for (int x = rect.tl.x; x < rect.br.x; x++)
        {
            const Complex c = calculate_complex(x, y, zoom);
            result[x + y * zoom->width] = mandelbrot_iterations(c, max_iterations);
        }
    }
}

void calculate_rect_with_period_check(const Rectangle rect, long double *result, const Zoom *zoom, const uint16_t max_iterations)
{
    const long double center_x = zoom->width / 2.0;
    const long double center_y = zoom->height / 2.0;
    const long double inv_factor = 1.0 / zoom->factor;

    for (int y = rect.tl.y; y < rect.br.y; y++)
    {
        const long double c_imag = (y - center_y) * inv_factor + zoom->offset_y;

        for (int x = rect.tl.x; x < rect.br.x; x++)
        {
            const long double c_real = (x - center_x) * inv_factor + zoom->offset_x;

            long double zr = 0.0, zi = 0.0;
            long double old_zr = 0.0, old_zi = 0.0;
            long double iter = 0;
            long double period = 1;
            long double check = 3;

            while ((uint16_t)iter < max_iterations)
            {
                long double zr2 = zr * zr;
                long double zi2 = zi * zi;

                if (zr2 + zi2 > 4.0)
                    break;

                zi = 2.0 * zr * zi + c_imag;
                zr = zr2 - zi2 + c_real;
                iter++;

                // if (check - 0.05 >= iter && iter >= check + 0.05 )
                if (check == iter)
                {
                    if (zr == old_zr && zi == old_zi)
                    {
                        iter = max_iterations;
                        break;
                    }
                    old_zr = zr;
                    old_zi = zi;
                    check = iter + period;
                    period *= 2;
                }
            }

            result[x + y * zoom->width] = iter;
        }
    }
}