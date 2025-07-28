cmake_minimum_required(VERSION 3.16)
project(mandelbrot C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

find_package(SDL2 QUIET)

if(NOT SDL2_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2 QUIET sdl2)
    endif()
endif()

if(NOT SDL2_FOUND)
    find_path(SDL2_INCLUDE_DIRS
            NAMES SDL.h
            PATHS
            /usr/local/include/SDL2
            /opt/homebrew/include/SDL2
            /usr/include/SDL2
            /mingw64/include/SDL2
            ${SDL2_DIR}/include
            PATH_SUFFIXES SDL2
    )

    find_library(SDL2_LIBRARIES
            NAMES SDL2
            PATHS
            /usr/local/lib
            /opt/homebrew/lib
            /usr/lib
            /mingw64/lib
            ${SDL2_DIR}/lib
    )

    if(SDL2_INCLUDE_DIRS AND SDL2_LIBRARIES)
        set(SDL2_FOUND TRUE)
    endif()
endif()

if(NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 not found! Please install SDL2 or set SDL2_DIR to the SDL2 installation directory.")
endif()

set(SOURCES
        main.c
        mandelbrot.c
        graphics.c
        mandelbrot_standard.c
        mandelbrot_iteration.c
        queue.c
        mandelbrot_four_split.c
)

add_executable(mandelbrot ${SOURCES})

target_compile_options(mandelbrot PRIVATE
        -Wall
        -Wextra
        -pg
        -O3
        -ffast-math
        -march=native
        -mtune=native
)

target_include_directories(mandelbrot PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(mandelbrot
        ${SDL2_LIBRARIES}
        Threads::Threads
        OpenGL::GL
        m
)

target_link_options(mandelbrot PRIVATE -pg)

target_compile_definitions(mandelbrot PRIVATE ${SDL2_CFLAGS_OTHER})