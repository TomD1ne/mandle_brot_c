#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL.h>

typedef struct
{
    unsigned char r, g, b, a;
} Color;

typedef struct
{
    float r, g, b, a;
} Point_Color;

typedef struct
{
    float x, y;
    float r, g, b, a;
} Graphics_Point;

typedef struct
{
    SDL_Window *window;
    SDL_GLContext gl_context;
    int width, height;

    Graphics_Point *points;
    int point_count;
    int point_buffer_size;
} Graphics;

Graphics *graphics_init(int width, int height, const char *title);
void graphics_destroy(Graphics *gfx);
void graphics_clear(Graphics *gfx);
void graphics_present(Graphics *gfx);

void graphics_add_point(Graphics *gfx, float x, float y, float r, float g, float b, float a);
void graphics_upload_points(Graphics *gfx, Graphics_Point *points, int count);
void graphics_reserve_points(Graphics *gfx, int count);
void graphics_flush_points(Graphics *gfx);

void graphics_draw_pixel(Graphics *gfx, int x, int y, Color color);

#define COLOR_FLOAT(r, g, b, a) (r), (g), (b), (a)
#define COLOR_BYTE_TO_FLOAT(r, g, b, a) ((r) / 255.0f), ((g) / 255.0f), ((b) / 255.0f), ((a) / 255.0f)

#endif