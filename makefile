CC = gcc

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    CFLAGS = -I/opt/homebrew/include/SDL2 -Wall -Wextra -pg -O3 -ffast-math -march=native -mtune=native
    LIBS = -L/opt/homebrew/lib -lpthread -lSDL2 -framework OpenGL -lm
else ifeq ($(UNAME_S),Linux)
    CFLAGS = $(shell pkg-config --cflags sdl2) -Wall -Wextra -pg -O3 -ffast-math -march=native -mtune=native -g -fsanitize=address
    LIBS = -lpthread $(shell pkg-config --libs sdl2) -lGL -lm
endif

FILES = main.c mandelbrot.c graphics.c mandelbrot_standard.c mandelbrot_iteration.c queue.c mandelbrot_four_split.c

mandelbrot: $(FILES)
	$(CC) $(FILES) $(CFLAGS) $(LIBS) -o mandelbrot

clean:
	rm -f mandelbrot

.PHONY: clean