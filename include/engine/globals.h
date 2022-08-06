#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <SDL2/SDL.h>

#define false   0
#define true    1
typedef unsigned char bool;

#define TILE_SIZE   64

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

typedef struct globals_Texture {
    SDL_Texture* texture;
    SDL_Rect size;
} globals_Texture;

typedef unsigned int size;

const SDL_Rect tile_size;
const SDL_Rect screen_size;

#endif