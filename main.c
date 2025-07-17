#include "graphics.h"
#include "mandelbrot.h"
#include "mandelbrot_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void screen_to_complex(int screen_x, int screen_y, int width, int height, Zoom *zoom, double *real, double *imag)
{
    *real = (screen_x - width / 2.0) / zoom->factor + zoom->offset_x;
    *imag = (screen_y - height / 2.0) / zoom->factor + zoom->offset_y;
}

void render_mandelbrot(Graphics *gfx, Zoom *zoom, int max_iterations)
{
    clock_t start = clock();
    int width = gfx->width;
    int height = gfx->height;

    printf("Rendering at zoom: %.0fx, offset: (%.6f, %.6f)",
           zoom->factor, zoom->offset_x, zoom->offset_y);

    graphics_clear(gfx);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Complex c;
            c.real = (x - width / 2.0) / zoom->factor + zoom->offset_x;
            c.imag = (y - height / 2.0) / zoom->factor + zoom->offset_y;

            int iterations = mandelbrot_iterations(c, max_iterations);
            graphics_draw_pixel(gfx, x, y, iterations, max_iterations);
        }
    }
    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    printf(" in %0.3f seconds\n", seconds);
    graphics_present(gfx);
}

int main(int argc, char *argv[])
{
    const int WIDTH = 1024;
    const int HEIGHT = 1024;
    const int MAX_ITERATIONS = 1000;

    Graphics *gfx = graphics_init(WIDTH, HEIGHT, "Mandelbrot Set - Click to Zoom");
    if (!gfx)
        return 1;

    Zoom zoom = {200.0, -0.5, 0.0};

    render_mandelbrot(gfx, &zoom, MAX_ITERATIONS);

    printf("Controls:\n");
    printf("- Left click: Zoom in at cursor position\n");
    printf("- Right click: Zoom out at cursor position\n");
    printf("- R key: Reset to initial view\n");
    printf("- ESC or close window: Exit\n");

    SDL_Event e;
    int quit = 0;
    int needs_redraw = 0;

    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_QUIT:
                quit = 1;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    double click_real, click_imag;
                    screen_to_complex(e.button.x, e.button.y, WIDTH, HEIGHT, &zoom, &click_real, &click_imag);

                    zoom.offset_x = click_real;
                    zoom.offset_y = click_imag;
                    zoom.factor *= 2.0;
                    needs_redraw = 1;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    double click_real, click_imag;
                    screen_to_complex(e.button.x, e.button.y, WIDTH, HEIGHT, &zoom, &click_real, &click_imag);

                    zoom.offset_x = click_real;
                    zoom.offset_y = click_imag;
                    zoom.factor /= 2.0;
                    needs_redraw = 1;
                }
                break;

            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                case SDLK_r:
                    zoom.factor = 200.0;
                    zoom.offset_x = -0.5;
                    zoom.offset_y = 0.0;
                    needs_redraw = 1;
                    break;
                case SDLK_ESCAPE:
                    quit = 1;
                    break;
                }
                break;
            }
        }

        if (needs_redraw)
        {
            clock_t start = clock();
            render_mandelbrot(gfx, &zoom, MAX_ITERATIONS);

            needs_redraw = 0;
        }

        SDL_Delay(16);
    }

    graphics_destroy(gfx);
    return 0;
}