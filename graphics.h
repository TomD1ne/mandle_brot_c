#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL.h>

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width;
    int height;
} Graphics;

Graphics *graphics_init(int width, int height, const char *title);
void graphics_destroy(Graphics *gfx);
void graphics_clear(Graphics *gfx);
void graphics_present(Graphics *gfx);
void graphics_draw_pixel(Graphics *gfx, int x, int y, int iterations, int max_iterations);

#endif