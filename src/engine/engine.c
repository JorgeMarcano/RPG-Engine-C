#include <stdio.h>
#include <stdlib.h>

#include "Engine/engine.h"
#include "Engine/scene.h"

void engine_mainloop(Engine* engine);
void handle_event(SDL_Event* event);
void mainloop_handle_keyboard(Engine* engine, SDL_Event* event);

int engine_init(EngineDesc* desc, Engine** p_engine) {

    //Initialize SDL
    if( SDL_Init( desc->subsystemFlags ) < 0 ) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    Engine* engine = malloc(sizeof(Engine));
    (*p_engine) = NULL;

    //Create window
    engine->window = SDL_CreateWindow( desc->title,
                                        desc->x,
                                        desc->y,
                                        desc->w,
                                        desc->h,
                                        desc->windowFlags );
    if( engine->window == NULL ) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        free(engine);
        return -2;
    }

    engine->renderer = SDL_CreateRenderer(engine->window, -1, SDL_RENDERER_ACCELERATED);
    if (engine->renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        engine_clean(engine);
        free(engine);
        return -3;
    }

    // Set render color
    SDL_SetRenderDrawColor(engine->renderer, 0x00, 0x00, 0x00, 0x00);

    //Get window surface
    engine->screenSurface = SDL_GetWindowSurface( engine->window );

    engine->is_running = false;
    engine->width = desc->w;
    engine->height = desc->h;

    (*p_engine) = engine;

    return 0;
}

void engine_clean(Engine* engine) {
    // If Scene not already cleared, destroy
    if (engine->scene)
        scene_destroy(engine->scene, true);

    // Destroy Renderer
    if (engine->renderer)
        SDL_DestroyRenderer(engine->renderer);

    //Destroy window
    if (engine->window)
        SDL_DestroyWindow( engine->window );

    engine->window = NULL;
    engine->screenSurface = NULL;
    engine->renderer = NULL;

    //Quit SDL subsystems
    SDL_Quit();

    free(engine);
}

void engine_clean_texture(globals_Texture* texture) {
    if (texture) {
        if (texture->texture) {
            SDL_DestroyTexture(texture->texture);
            texture->texture = NULL;
        }

        free(texture);
    }
}

void engine_mainloop(Engine* engine) {
    if (!engine)
        return;

    engine->is_running = true;

    // TODO: Might have to add guard for overflow (>1000Hrs though so not priority)
    Uint32 t0 = SDL_GetTicks();
    Uint32 t1 = t0;

    SDL_Event event;
    while (engine->is_running) {
        // Handle all events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                engine->is_running = false;
                break;
            case SDL_KEYDOWN:
                mainloop_handle_keyboard(engine, &event);
                break;
            }

            // Have Scene handle event
            scene_handle_event(engine->scene, &event);
        }

        // Render clear
        SDL_SetRenderDrawColor(engine->renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(engine->renderer);
        SDL_SetRenderDrawColor(engine->renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        // Update and Render Scene
        t1 = SDL_GetTicks();
        scene_update_render(engine->scene, t1-t0);

        // Update screen
        SDL_RenderPresent(engine->renderer);

        // Update old time
        t0 = t1;
    }
    
    return;
}

void load_bmp(Engine* engine, const char* src, globals_Texture** dest) {
    SDL_Surface* surface;

    surface = SDL_LoadBMP(src);
    if (surface == NULL) {
        printf("Unable to load image %s! SDL Error: %s\n", src, SDL_GetError());
        (*dest) = NULL;
        return;
    }

    (*dest) = malloc(sizeof(globals_Texture));
    (*dest)->texture = SDL_CreateTextureFromSurface(engine->renderer, surface);
    if ((*dest)->texture == NULL) {
        printf("Failed to create texture from %s! SDL Error: %s\n", src, SDL_GetError());
        free(*dest);
        (*dest) = NULL;
    }
    else {
        (*dest)->size.x = 0;
        (*dest)->size.y = 0;
        (*dest)->size.w = surface->w;
        (*dest)->size.h = surface->h;
    }

    SDL_FreeSurface(surface);

    return;
}

void mainloop_handle_keyboard(Engine* engine, SDL_Event* event) {
    switch( event->key.keysym.sym )
    {
    case SDLK_UP:
        printf("UP\n");
        break;

    case SDLK_DOWN:
        printf("DOWN\n");
        break;

    case SDLK_LEFT:
        printf("LEFT\n");
        break;

    case SDLK_RIGHT:
        printf("RIGHT\n");
        break;

    case SDLK_ESCAPE:
        engine->is_running = false;
        break;

    default:
        printf("Default Key\n");
    }
}

void engine_start(Engine* engine, Scene* scene) {
    // TODO: Load scene from file
    scene_load(scene, &(engine->scene));

    // Start Mainloop
    engine_mainloop(engine);
}