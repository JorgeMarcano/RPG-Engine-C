#include <stdio.h>
#include <stdlib.h>

#include "engine.h"

//Screen dimension constants
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(int argc, char* argv[]) {

	EngineDesc desc;
    desc.subsystemFlags = SDL_INIT_VIDEO;
    desc.title = "RPG Engine";
    desc.x = SDL_WINDOWPOS_UNDEFINED;
    desc.y = SDL_WINDOWPOS_UNDEFINED;
    desc.w = SCREEN_WIDTH;
    desc.h = SCREEN_HEIGHT;
    desc.windowFlags = SDL_WINDOW_SHOWN;

    Engine* engine;

    engine_init(&desc, &engine);

    //Fill the surface white
    SDL_FillRect( engine->screenSurface, NULL, SDL_MapRGB( engine->screenSurface->format, 0xFF, 0xFF, 0xFF ) );
    
    //Update the surface
    SDL_UpdateWindowSurface( engine->window );

    //Wait two seconds
    SDL_Delay( 2000 );

    engine_clean(engine);

	return 0;
}