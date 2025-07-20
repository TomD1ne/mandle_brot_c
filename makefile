CC = gcc
CFLAGS = -I/opt/homebrew/include/SDL2 -Wall -Wextra -pg
LIBS = -L/opt/homebrew/lib -lSDL2 -lpthread

mandelbrot: main.c mandelbrot.c graphics.c mandelbrot_standard.c mandelbrot_iteration.c queue.c
	$(CC) main.c mandelbrot.c graphics.c mandelbrot_standard.c mandelbrot_iteration.c queue.c $(CFLAGS) $(LIBS) -o mandelbrot

clean:
	rm -f mandelbrot

.PHONY: clean