#include "graphics.h"
#include "mandelbrot.h"
#include "mandelbrot_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void screen_to_complex(int screen_x, int screen_y, Zoom *zoom, double *real, double *imag)
{
    *real = (screen_x - zoom->width / 2.0) / zoom->factor + zoom->offset_x;
    *imag = (screen_y - zoom->height / 2.0) / zoom->factor + zoom->offset_y;
}

void color_madelbrot_pixel(Graphics *gfx, int x, int y, int iterations, uint16_t max_iterations)
{
    Color color;
    if (iterations == max_iterations)
    {
        color = (Color){0, 0, 0, 255};
    }
    else
    {
        double log_iterations = log(iterations + 1) / log(max_iterations + 1);
        int temp = (int)(log_iterations * 255 * 3);
        int r = temp & ((1 << 8) - 1) << 16;
        int g = temp & ((1 << 8) - 1) << 8;
        int b = temp & ((1 << 8) - 1);
        color = (Color){r, g, b, 255};
    }
    graphics_draw_pixel(gfx, x, y, color);
}

void render_mandelbrot(Graphics *gfx, Zoom *zoom, uint16_t max_iterations)
{
    int test_count = 30;
    float best_time = 9999999;
    int best_thread_count = 0;
    float total_time = 0;
    clock_t end = clock();

    int width = zoom->width;
    int height = zoom->height;

    for (int i = 1; i < test_count; i++)
    {
        clock_t start = clock();

        printf("Rendering at zoom: %.0fx, offset: (%.3f, %.3f) ",
               zoom->factor, zoom->offset_x, zoom->offset_y);

        graphics_clear(gfx);

        clock_t start_iterating = clock();

        uint16_t *result = malloc(width * height * (sizeof(uint16_t)));
        calculate_mandelbrot(*zoom, max_iterations, result, STANDARD, i);

        end = clock();
        float seconds = (float)(end - start_iterating) / CLOCKS_PER_SEC;
        // printf("Calculated mandlebrot in %0.3f seconds \n", seconds);

        clock_t start_clouring = clock();

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                color_madelbrot_pixel(gfx, x, y, result[x + y * width], max_iterations);
            }
        }

        end = clock();
        seconds = (float)(end - start_clouring) / CLOCKS_PER_SEC;
        // printf("Coloured mandlebrot in %0.3f seconds \n", seconds);

        end = clock();
        seconds = (float)(end - start) / CLOCKS_PER_SEC;
        total_time += seconds;
        if (seconds < best_time)
        {
            best_time = seconds;
            best_thread_count = i;
        }
        printf("rendered in %0.3f seconds with %i thread(s)\n", seconds, i);
        graphics_present(gfx);
    }

    printf("Average rendering time: %0.3f\n", total_time / (test_count - 1));
    printf("Best rendering time: %0.3f with %i thread(s) \n\n", best_time, best_thread_count);
    // exit(0);
}

int main(int argc, char *argv[])
{
    const int WIDTH = 800;
    const int HEIGHT = 800;
    const uint16_t MAX_ITERATIONS = 1000;

    Graphics *gfx = graphics_init(WIDTH, HEIGHT, "Mandelbrot Set - Click to Zoom");
    if (!gfx)
        return 1;

    Zoom zoom = {200.0, -0.5, 0.0, WIDTH, HEIGHT};

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
                    screen_to_complex(e.button.x, e.button.y, &zoom, &click_real, &click_imag);

                    zoom.offset_x = click_real;
                    zoom.offset_y = click_imag;
                    zoom.factor *= 2.0;
                    needs_redraw = 1;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    double click_real, click_imag;
                    screen_to_complex(e.button.x, e.button.y, &zoom, &click_real, &click_imag);

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