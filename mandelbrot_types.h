#ifndef MANDELBROT_TYPES_H
#define MANDELBROT_TYPES_H

typedef struct
{
    long double real;
    long double imag;
} Complex;

typedef struct
{
    long double factor;
    long double offset_x;
    long double offset_y;
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