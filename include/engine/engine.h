#ifndef ENGINE_H
#define ENGINE_H

#include "globals.h"
#include "scene.h"

typedef struct Engine {
    // Graphics based stuff
    SDL_Window*     window;
    SDL_Surface*    screenSurface;
    SDL_Renderer*   renderer;

    int width;
    int height;

    // Status information
    bool is_running;

    // Game information
    Scene* scene;
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
void engine_start(Engine* engine, Scene* scene); // FIXME: Temp parameter, will later be file

void engine_clean(Engine* engine);

void load_bmp(Engine* engine, const char* src, globals_Texture** dest);
void engine_clean_texture(globals_Texture* texture);

#endif