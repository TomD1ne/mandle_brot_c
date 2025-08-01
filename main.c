#include "graphics.h"
#include "mandelbrot.h"
#include "mandelbrot_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <tgmath.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <SDL.h>

static void screen_to_complex(const int screen_x, const int screen_y, const Zoom *zoom, long double *real, long double *imag)
{
    *real = (screen_x - zoom->width / 2.0) / zoom->factor + zoom->offset_x;
    *imag = (screen_y - zoom->height / 2.0) / zoom->factor + zoom->offset_y;
}

Point_Color hsv_to_rgb(float h, float s, float v, float a)
{
    float c = v * s;
    float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r, g, b;
    if (h < 60.0f)
    {
        r = c;
        g = x;
        b = 0;
    }
    else if (h < 120.0f)
    {
        r = x;
        g = c;
        b = 0;
    }
    else if (h < 180.0f)
    {
        r = 0;
        g = c;
        b = x;
    }
    else if (h < 240.0f)
    {
        r = 0;
        g = x;
        b = c;
    }
    else if (h < 300.0f)
    {
        r = x;
        g = 0;
        b = c;
    }
    else
    {
        r = c;
        g = 0;
        b = x;
    }

    return (Point_Color){r + m, g + m, b + m, a};
}

void init_palette_advanced(const uint16_t max_iterations, Point_Color *palette)
{
    for (uint16_t i = 0; i <= max_iterations; ++i)
    {
        if (i == max_iterations)
        {
            palette[i] = (Point_Color){0.0f, 0.0f, 0.0f, 1.0f};
            continue;
        }

        const long double t = (long double)i / max_iterations;

        float r = 0.5f + 0.25f * sinf(t * 3.14159f * 7.0f) +
                  0.15f * sinf(t * 3.14159f * 13.0f) +
                  0.1f * sinf(t * 3.14159f * 23.0f);

        float g = 0.5f + 0.25f * sinf(t * 3.14159f * 11.0f + 2.1f) +
                  0.15f * sinf(t * 3.14159f * 17.0f + 1.3f) +
                  0.1f * sinf(t * 3.14159f * 29.0f + 0.7f);

        float b = 0.5f + 0.25f * sinf(t * 3.14159f * 5.0f + 4.2f) +
                  0.15f * sinf(t * 3.14159f * 19.0f + 3.8f) +
                  0.1f * sinf(t * 3.14159f * 31.0f + 2.4f);

        float spiral = sinf(t * 3.14159f * 20.0f) * expf(-t * 2.0f);
        r += spiral * 0.2f;
        g += spiral * 0.15f;
        b += spiral * 0.1f;

        r = fmaxf(0.0f, fminf(r, 1.0f));
        g = fmaxf(0.0f, fminf(g, 1.0f));
        b = fmaxf(0.0f, fminf(b, 1.0f));

        palette[i] = (Point_Color){r, g, b, 1.0f};
    }
}

void init_palette(const uint16_t max_iterations, Point_Color *palette)
{
    for (uint16_t i = 0; i <= max_iterations; ++i)
    {
        if (i == max_iterations)
        {
            palette[i] = (Point_Color){0.0f, 0.0f, 0.0f, 1.0f};
            continue;
        }

        const long double t = (long double)i / max_iterations;

        float r = sin(3.0f + t * 12.0f) * 0.5f + 0.5f;
        float g = sin(2.1f + t * 12.0f) * 0.5f + 0.5f;
        float b = sin(1.0f + t * 12.0f) * 0.5f + 0.5f;

        const float brightness = pow(t, 0.3);
        r *= brightness;
        g *= brightness;
        b *= brightness;

        palette[i] = (Point_Color){r, g, b, 1.0f};
    }
}

void render_mandelbrot(Graphics *gfx, const Zoom *zoom, const uint16_t max_iterations, const Point_Color *palette, uint16_t thread_count, enum MandelbrotStrategy strategy)
{
    const int width = zoom->width;
    const int height = zoom->height;

    graphics_clear(gfx);

    uint16_t *result = malloc(width * height * (sizeof(uint16_t)));
    calculate_mandelbrot(*zoom, max_iterations, result, strategy, thread_count);

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

void render_mandelbrot_benchmark(Graphics *gfx, const Zoom *zoom, uint16_t max_iterations, const Point_Color *palette, int test_count, enum MandelbrotStrategy strategy)
{
    long double best_time = 9999999;
    int best_thread_count = 0;
    long double total_time = 0;

    const int width = zoom->width;
    const int height = zoom->height;

    struct timespec start, end;

    for (int i = 1; i < test_count; i++)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);

        printf("Rendering at zoom: %.0fx, offset: (%.3f, %.3f) ",
               zoom->factor, zoom->offset_x, zoom->offset_y);

        graphics_clear(gfx);

        uint16_t *result = malloc(width * height * (sizeof(uint16_t)));
        calculate_mandelbrot(*zoom, max_iterations, result, strategy, i);

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
        // sleep(1);
        clock_gettime(CLOCK_MONOTONIC, &end);
        const long double seconds = (long double)(end.tv_sec - start.tv_sec + end.tv_nsec - start.tv_nsec) / 1e9;
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
    const int WIDTH = 1430;
    const int HEIGHT = 850;
    const int thread_count = 10;
    const int test_count = 30;
    const uint16_t MAX_ITERATIONS = 1000;

    Point_Color palette[MAX_ITERATIONS + 1];
    init_palette_advanced(MAX_ITERATIONS, palette);

    Graphics *gfx = graphics_init(WIDTH, HEIGHT, "Mandelbrot Set - Click to Zoom");
    if (!gfx)
        return 1;

    graphics_reserve_points(gfx, WIDTH * HEIGHT);

    Zoom zoom = {200.0, -0.500, 0.0, WIDTH, HEIGHT};

    render_mandelbrot(gfx, &zoom, MAX_ITERATIONS, palette, test_count, FOUR_SPLIT);

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
                    long double click_real, click_imag;
                    screen_to_complex(e.button.x, e.button.y, &zoom, &click_real, &click_imag);

                    zoom.offset_x = click_real;
                    zoom.offset_y = click_imag;
                    zoom.factor *= 2.0;
                    needs_redraw = 1;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    long double click_real, click_imag;
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
                default:;
                }
                break;
            default:
                break;
                ;
            }
        }

        if (needs_redraw)
        {
            render_mandelbrot(gfx, &zoom, MAX_ITERATIONS, palette, thread_count, FOUR_SPLIT);
            needs_redraw = 0;
        }

        SDL_Delay(16);
    }

    graphics_destroy(gfx);
    return 0;
}