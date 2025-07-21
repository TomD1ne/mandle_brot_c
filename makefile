CC = gcc
CFLAGS = -I/opt/homebrew/include/SDL2 -Wall -Wextra -pg -fsanitize=address -g -O1 -ffast-math -march=native -mtune=native
LIBS = -L/opt/homebrew/lib -lpthread -lSDL2 -framework OpenGL -lm
FILES = main.c mandelbrot.c graphics.c mandelbrot_standard.c mandelbrot_iteration.c queue.c mandelbrot_four_split.c

mandelbrot: $(FILES)
	$(CC) $(FILES) $(CFLAGS) $(LIBS) -o mandelbrot

clean:
	rm -f mandelbrot

.PHONY: clean