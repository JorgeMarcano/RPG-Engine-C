#include <stdio.h>
#include <stdlib.h>

#include "Engine/engine.h"
#include "Engine/scene.h"

void engine_mainloop(Engine* engine);

Uint32 scene_event_id = 0;

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

    engine->renderer = SDL_CreateRenderer(engine->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
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

    engine->fctMapping = desc->fctMapping;

    scene_event_id = SDL_RegisterEvents(1);
    if (scene_event_id == (Uint32)-1) {
        printf("Could not register a custom event! SDL Error: %s\n", SDL_GetError());
        engine_clean(engine);
        free(engine);
        return -4;
    }

    (*p_engine) = engine;

    return 0;
}

void engine_clean(Engine* engine) {
    // If Scene not already cleared, destroy
    if (engine->scene)
        scene_destroy(engine->scene, true);

    // Remove all tilesets
    globals_Tileset* currNode = engine->tilesets.next;
    globals_Tileset* nextNode;
    while (currNode != NULL) {
        nextNode = currNode->next;
        files_clean_tileset(currNode);
        currNode = nextNode;
    }

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

void engine_mainloop(Engine* engine) {
    if (!engine)
        return;

    engine->is_running = true;

    Uint32 t0 = SDL_GetTicks();
    Uint32 t1 = t0;
    Uint32 deltaT = 0;

    SDL_Event event;
    while (engine->is_running) {
        // Handle all events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                engine->is_running = false;
                break;
            }
            else if (event.type == scene_event_id) {
                if (event.user.code == SCENE_EVENT_CHANGE_HUD_KEEP || event.user.code == SCENE_EVENT_CHANGE_HUD_REMOVE) {
                    // Old scene is in data1, door is in data2
                    files_load_scene(((Door*) event.user.data2)->scene_src,
                                        ((Door*) event.user.data2)->spawnID,
                                        &(engine->scene),
                                        engine->renderer,
                                        &(engine->tilesets),
                                        engine->fctMapping);

                    if (event.user.code == SCENE_EVENT_CHANGE_HUD_KEEP)
                        engine->scene->hud = ((Scene*) event.user.data1)->hud;

                    scene_destroy(event.user.data1, event.user.code == SCENE_EVENT_CHANGE_HUD_REMOVE);
                }
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
        // Handle timer overflow
        if (t1 < t0)
            deltaT = ((SDL_MAX_UINT32) - t0) + t1;
        else
            deltaT = t1 - t0;

        scene_update_render(engine->scene, deltaT);

        // Update screen
        SDL_RenderPresent(engine->renderer);

        // Update old time
        t0 = t1;
    }
    
    return;
}

void engine_start(Engine* engine, Door spawnDoor) {
    // Load scene from file
    files_load_scene(spawnDoor.scene_src,
                        spawnDoor.spawnID,
                        &(engine->scene),
                        engine->renderer,
                        &(engine->tilesets),
                        engine->fctMapping);

    // Start Mainloop
    engine_mainloop(engine);
}

void engine_render(SDL_Renderer* renderer, SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect, unsigned int scale) {
    SDL_Rect scaled_rect;
    scaled_rect.x = dstrect->x * scale;
    scaled_rect.y = dstrect->y * scale;
    scaled_rect.w = dstrect->w * scale;
    scaled_rect.h = dstrect->h * scale;

    SDL_RenderCopy(renderer, texture, srcrect, &scaled_rect);
}