#include <stdio.h>
#include <stdlib.h>

#include "engine.h"

int engine_init(EngineDesc* desc, Engine** p_engine) {

    //Initialize SDL
    if( SDL_Init( desc->subsystemFlags ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return -1;
    }

    Engine* engine = malloc(sizeof(Engine));
    (*p_engine) = engine;

    //Create window
    engine->window = SDL_CreateWindow( desc->title,
                                        desc->x,
                                        desc->y,
                                        desc->w,
                                        desc->h,
                                        desc->windowFlags );
    if( engine->window == NULL )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        free(engine);
        (*p_engine) = NULL;
        return -2;
    }

    //Get window surface
    engine->screenSurface = SDL_GetWindowSurface( engine->window );

    return 0;
}

const char* engine_getError() {
    return SDL_GetError();
}

void engine_clean(Engine* engine) {
    //Destroy window
    SDL_DestroyWindow( engine->window );

    //Quit SDL subsystems
    SDL_Quit();

    free(engine);
}