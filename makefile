CC = gcc
CFLAGS = -I/opt/homebrew/include/SDL2 -Wall -Wextra -pg
LIBS = -L/opt/homebrew/lib -lSDL2 -lpthread
FILES = main.c mandelbrot.c graphics.c mandelbrot_standard.c mandelbrot_iteration.c queue.c mandelbrot_four_split.c

mandelbrot: $(FILES)
	$(CC) $(FILES) $(CFLAGS) $(LIBS) -o mandelbrot

clean:
	rm -f mandelbrot

.PHONY: clean