#include <stdio.h>
#include <stdlib.h>

#include "Engine/engine.h"

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
    
    engine_mainloop(engine);

    engine_clean(engine);

	return 0;
}