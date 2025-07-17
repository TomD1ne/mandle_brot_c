#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>

Graphics *graphics_init(int width, int height, const char *title)
{
    Graphics *gfx = malloc(sizeof(Graphics));
    if (!gfx)
        return NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL init failed: %s\n", SDL_GetError());
        free(gfx);
        return NULL;
    }

    gfx->window = SDL_CreateWindow(title,
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   width, height, SDL_WINDOW_SHOWN);

    if (!gfx->window)
    {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        free(gfx);
        return NULL;
    }

    gfx->renderer = SDL_CreateRenderer(gfx->window, -1, SDL_RENDERER_ACCELERATED);
    if (!gfx->renderer)
    {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(gfx->window);
        SDL_Quit();
        free(gfx);
        return NULL;
    }

    gfx->width = width;
    gfx->height = height;

    return gfx;
}

void graphics_destroy(Graphics *gfx)
{
    if (gfx)
    {
        SDL_DestroyRenderer(gfx->renderer);
        SDL_DestroyWindow(gfx->window);
        SDL_Quit();
        free(gfx);
    }
}

void graphics_clear(Graphics *gfx)
{
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);
    SDL_RenderClear(gfx->renderer);
}

void graphics_present(Graphics *gfx)
{
    SDL_RenderPresent(gfx->renderer);
}

void graphics_draw_pixel(Graphics *gfx, int x, int y, int iterations, int max_iterations)
{
    if (iterations == max_iterations)
    {
        SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255); // Black for points in set
    }
    else
    {
        double log_iterations = log(iterations + 1) / log(max_iterations + 1);
        int color = (int)(log_iterations * 255);
        SDL_SetRenderDrawColor(gfx->renderer, 0, color / 2, color, 255);
    }
    SDL_RenderDrawPoint(gfx->renderer, x, y);
}