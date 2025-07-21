#include "graphics.h"
#include "mandelbrot.h"
#include "mandelbrot_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static inline void screen_to_complex(int screen_x, int screen_y, Zoom *zoom, double *real, double *imag)
{
    *real = (screen_x - zoom->width / 2.0) / zoom->factor + zoom->offset_x;
    *imag = (screen_y - zoom->height / 2.0) / zoom->factor + zoom->offset_y;
}

void init_palette(uint16_t max_iterations, Point_Color *palette)
{
    for (uint16_t i = 0; i <= max_iterations; ++i)
    {
        if (i == max_iterations)
        {
            palette[i] = (Point_Color){0.0f, 0.0f, 0.0f, 1.0f};
            continue;
        }

        double t = (double)i / max_iterations;

        double freq1 = 2.0 * M_PI * t;
        double freq2 = 4.0 * M_PI * t;
        double freq3 = 6.0 * M_PI * t;

        float r = 0.5 + 0.5 * sin(freq1 + 0.0);
        float g = 0.5 + 0.5 * sin(freq2 + 2.094);
        float b = 0.5 + 0.5 * sin(freq3 + 4.188);

        double brightness = pow(t, 0.3);
        r = r * brightness;
        g = g * brightness;
        b = b * brightness;

        palette[i] = (Point_Color){r, g, b, 1.0f};
    }
}

void render_mandelbrot(Graphics *gfx, Zoom *zoom, uint16_t max_iterations, Point_Color *palette, uint16_t thread_count)
{
    int width = zoom->width;
    int height = zoom->height;

    graphics_clear(gfx);

    uint16_t *result = malloc(width * height * (sizeof(uint16_t)));
    calculate_mandelbrot(*zoom, max_iterations, result, FOUR_SPLIT, thread_count);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Point_Color color = palette[result[x + y * width]];
            Graphics_Point *p = &gfx->points[gfx->point_count];
            p->x = (float)x;
            p->y = (float)y;
            p->r = color.r;
            p->g = color.g;
            p->b = color.b;
            p->a = color.a;

            gfx->point_count++;
        }
    }

    graphics_present(gfx);
    free(result);
}

void render_mandelbrot_benchmark(Graphics *gfx, Zoom *zoom, uint16_t max_iterations, Point_Color *palette, int test_count)
{
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

        uint16_t *result = malloc(width * height * (sizeof(uint16_t)));
        calculate_mandelbrot(*zoom, max_iterations, result, FOUR_SPLIT, i);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                Point_Color color = palette[result[x + y * width]];
                Graphics_Point *p = &gfx->points[gfx->point_count];
                p->x = (float)x;
                p->y = (float)y;
                p->r = color.r;
                p->g = color.g;
                p->b = color.b;
                p->a = color.a;

                gfx->point_count++;
            }
        }

        graphics_present(gfx);

        end = clock();
        float seconds = (float)(end - start) / CLOCKS_PER_SEC;
        total_time += seconds;
        if (seconds < best_time)
        {
            best_time = seconds;
            best_thread_count = i;
        }
        printf("rendered in %0.3f seconds with %i thread(s)\n", seconds, i);
        free(result);
    }

    if (test_count > 2)
    {
        printf("Average rendering time: %0.3f\n", total_time / (test_count - 1));
        printf("Best rendering time: %0.3f with %i thread(s) \n\n", best_time, best_thread_count);
    }
}

int main()
{
    const int WIDTH = 800;
    const int HEIGHT = 800;
    const int thread_count = 1;
    const uint16_t MAX_ITERATIONS = 1000;

    Point_Color palette[MAX_ITERATIONS + 1];
    init_palette(MAX_ITERATIONS, palette);

    Graphics *gfx = graphics_init(WIDTH, HEIGHT, "Mandelbrot Set - Click to Zoom");
    if (!gfx)
        return 1;

    graphics_reserve_points(gfx, WIDTH * HEIGHT);

    Zoom zoom = {200.0, -0.5, 0.0, WIDTH, HEIGHT};

    render_mandelbrot_benchmark(gfx, &zoom, MAX_ITERATIONS, palette, 2);

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
            render_mandelbrot_benchmark(gfx, &zoom, MAX_ITERATIONS, palette, 2);

            needs_redraw = 0;
        }

        SDL_Delay(16);
    }

    graphics_destroy(gfx);
    return 0;
}