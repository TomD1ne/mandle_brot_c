#include "mandelbrot_types.h"
#include "mandelbrot_iteration.h"
#include <pthread.h>

void calculate_mandlebrot_standard(int width, int height, Zoom z, int max_iterations, int *result, int num_threads, int chunck_size)
{
    int squares_in_width = width / chunck_size + (width % chunck_size != 0);
    int squares_in_height = height / chunck_size + (height % chunck_size != 0);

    Rectangle *rectangles = malloc(squares_in_height * squares_in_width * sizeof(Rectangle));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Complex c = calculate_complex(x, y, width, height, z);
            result[y * width + x] = mandelbrot_iterations(c, max_iterations);
        }
    }
}