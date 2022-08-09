#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <SDL2/SDL.h>

#define false   0
#define true    1
typedef unsigned char bool;

#define TILE_SIZE   16

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

typedef struct globals_Functional_Tileset {

    int firstgid;
    int tile_count;

    char** sourcesList;
    int* sourcesUseCount;

} globals_Functional_Tileset;

typedef struct globals_Tile {
    globals_Tileset* tileset;
    SDL_Point pos;
    int localTileID;
} globals_Tile;

// Updater  -> void foo(void* entity, void* scene, Uint32 deltaT)
typedef void (*Updater)(void*, void*, Uint32);
// Renderer -> void foo(void* entity, void* scene)
typedef void (*Renderer)(void*, void*);
// Handler  -> void foo(void* entity, void* scene, SDL_Event* event)
typedef void (*Handler)(void*, void*, SDL_Event*);

typedef struct FunctionMaps {
    const Updater* updaters;
    const Renderer* renderers;
    const Handler* handlers;
} FunctionMaps;

typedef unsigned int size;

const SDL_Rect tile_size;
const SDL_Rect screen_size;

struct Engine;
struct Scene;

Uint32 scene_event_id;
#define SCENE_EVENT_CHANGE    0x1

#endif