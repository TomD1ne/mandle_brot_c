#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <OpenGL/gl.h>

Graphics *graphics_init(int width, int height, const char *title)
{
    Graphics *gfx = malloc(sizeof(Graphics));
    if (!gfx)
        return NULL;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL init failed: %s\n", SDL_GetError());
        free(gfx);
        return NULL;
    }

    gfx->window = SDL_CreateWindow(title,
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   width, height,
                                   SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (!gfx->window)
    {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        free(gfx);
        return NULL;
    }

    gfx->gl_context = SDL_GL_CreateContext(gfx->window);
    if (!gfx->gl_context)
    {
        printf("OpenGL context creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(gfx->window);
        SDL_Quit();
        free(gfx);
        return NULL;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gfx->width = width;
    gfx->height = height;

    gfx->point_buffer_size = 10000;
    gfx->points = malloc(sizeof(Graphics_Point) * gfx->point_buffer_size);
    gfx->point_count = 0;

    return gfx;
}

void graphics_destroy(Graphics *gfx)
{
    if (gfx)
    {
        if (gfx->points)
            free(gfx->points);
        SDL_GL_DeleteContext(gfx->gl_context);
        SDL_DestroyWindow(gfx->window);
        SDL_Quit();
        free(gfx);
    }
}

void graphics_clear(Graphics *gfx)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    gfx->point_count = 0;
}

void graphics_present(Graphics *gfx)
{
    if (gfx->point_count > 0)
    {
        graphics_flush_points(gfx);
    }
    SDL_GL_SwapWindow(gfx->window);
}

void graphics_add_point(Graphics *gfx, float x, float y, float r, float g, float b, float a)
{
    if (gfx->point_count >= gfx->point_buffer_size)
    {
        gfx->point_buffer_size *= 2;
        gfx->points = realloc(gfx->points, sizeof(Graphics_Point) * gfx->point_buffer_size);
    }

    Graphics_Point *p = &gfx->points[gfx->point_count];
    p->x = x;
    p->y = y;
    p->r = r;
    p->g = g;
    p->b = b;
    p->a = a;

    gfx->point_count++;
}

void graphics_upload_points(Graphics *gfx, Graphics_Point *points, int count)
{
    if (count > gfx->point_buffer_size)
    {
        gfx->point_buffer_size = count + 1000;
        gfx->points = realloc(gfx->points, sizeof(Graphics_Point) * gfx->point_buffer_size);
    }

    memcpy(gfx->points, points, sizeof(Graphics_Point) * count);
    gfx->point_count = count;
}

void graphics_reserve_points(Graphics *gfx, int count)
{
    if (count > gfx->point_buffer_size)
    {
        gfx->point_buffer_size = count;
        gfx->points = realloc(gfx->points, sizeof(Graphics_Point) * gfx->point_buffer_size);
    }
}

void graphics_flush_points(Graphics *gfx)
{
    if (gfx->point_count == 0)
        return;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_FLOAT, sizeof(Graphics_Point), &gfx->points[0].x);

    glColorPointer(4, GL_FLOAT, sizeof(Graphics_Point), &gfx->points[0].r);
    glDrawArrays(GL_POINTS, 0, gfx->point_count);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    gfx->point_count = 0;
}

void graphics_draw_pixel(Graphics *gfx, int x, int y, Color color)
{
    graphics_add_point(gfx, (float)x, (float)y,
                       color.r / 255.0f, color.g / 255.0f,
                       color.b / 255.0f, color.a / 255.0f);
}