#ifndef MANDELBROT_TYPES_H
#define MANDELBROT_TYPES_H

typedef struct
{
    double real;
    double imag;
} Complex;

typedef struct
{
    double factor;
    double offset_x;
    double offset_y;
    int width;
    int height;
} Zoom;

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    Point tl;
    Point br;
} Rectangle;

#endif