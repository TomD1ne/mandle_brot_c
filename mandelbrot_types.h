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
} Zoom;

typedef struct
{
    Point tl;
    Point br;
} Rectangle;

typedef struct
{
    int x;
    int y;
} Point;

#endif