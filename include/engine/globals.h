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

typedef struct globals_Tileset {
    globals_Texture* texture;

    SDL_Rect size_tiles;

    char* set_name;
    char* src_name;

    Uint32 usages;

    struct globals_Tileset* prev;
    struct globals_Tileset* next;
} globals_Tileset;

typedef struct globals_Tile {
    globals_Tileset* tileset;
    SDL_Point pos;
} globals_Tile;

typedef unsigned int size;

const SDL_Rect tile_size;
const SDL_Rect screen_size;

struct Engine;

#endif