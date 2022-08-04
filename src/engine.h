#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>

typedef struct Engine {
    SDL_Window*     window;
    SDL_Surface*    screenSurface;
} Engine;

typedef struct EngineDesc {
    Uint32      subsystemFlags;

    const char* title;

    int         x;
    int         y;
    int         w;
    int         h;

    Uint32      windowFlags;
} EngineDesc;

int engine_init(EngineDesc* desc, Engine** p_engine);

const char* engine_getError();

void engine_clean(Engine* engine);

#endif