#include <SDL.h>

#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width;
    int height;
} Graphics;

typedef struct
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} Color;

Graphics *graphics_init(int width, int height, const char *title);
void graphics_destroy(Graphics *gfx);
void graphics_clear(Graphics *gfx);
void graphics_present(Graphics *gfx);
void graphics_draw_pixel(Graphics *gfx, int x, int y, Color color);

#endif