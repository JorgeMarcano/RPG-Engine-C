#include <stdio.h>
#include <stdlib.h>

#include "Engine/engine.h"

const Updater updaters[];
const Renderer renderers[];
const Handler handlers[];
const FunctionMaps mapping;

int main(int argc, char* argv[]) {

	EngineDesc desc;
    desc.subsystemFlags = SDL_INIT_VIDEO;
    desc.title = "RPG Engine";
    desc.x = SDL_WINDOWPOS_UNDEFINED;
    desc.y = SDL_WINDOWPOS_UNDEFINED;
    desc.w = SCREEN_WIDTH;
    desc.h = SCREEN_HEIGHT;
    desc.windowFlags = SDL_WINDOW_SHOWN;
    desc.fctMapping = &mapping;
    
    Engine* engine;

    // Engine* engine;

    Door startDoor = {.scene_src = "res/maps/test.tmx", .spawnID = 0};

    engine_init(&desc, &engine);
    
    engine_start(engine, startDoor);

    engine_clean(engine);

	return 0;
}

// TODO: Temp
SDL_Point speed;

void update_map(void* entity, void* scene, Uint32 deltaT) {
    static Uint32 lastUpdate = 0;
    lastUpdate += deltaT;

    Scene* p_scene = scene;
    Map* map = entity;

    if (lastUpdate > 100) {
        p_scene->center.x += speed.x;
        p_scene->center.y += speed.y;

        if (p_scene->center.x < 0)
            p_scene->center.x = 0;
        else if (p_scene->center.x >= p_scene->map->size_tile.w)
            p_scene->center.x = p_scene->map->size_tile.w - 1;

        if (p_scene->center.y < 0)
            p_scene->center.y = 0;
        else if (p_scene->center.y >= p_scene->map->size_tile.h)
            p_scene->center.y = p_scene->map->size_tile.h - 1;

        lastUpdate = 0;

        // printf("x:%d, y:%d\n", p_scene->center.x, p_scene->center.y);
    }

    Door* door;
    if (entity_check_door(p_scene->center, p_scene, &door))
        scene_change(p_scene, door, true);
}

void event_map(void* entity, void* scene, SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        switch( event->key.keysym.sym )
        {
        case SDLK_UP:
            speed.y = -1;
            break;

        case SDLK_DOWN:
            speed.y = 1;
            break;

        case SDLK_LEFT:
            speed.x = -1;
            break;

        case SDLK_RIGHT:
            speed.x = 1;
            break;
        }
    } else if (event->type == SDL_KEYUP) {
        switch( event->key.keysym.sym )
        {
        case SDLK_UP:
        case SDLK_DOWN:
            speed.y = 0;
            break;

        case SDLK_LEFT:
        case SDLK_RIGHT:
            speed.x = 0;
            break;
        }
    }
}

const Updater updaters[] = {update_map};
const Renderer renderers[] = {NULL};
const Handler handlers[] = {event_map};
const FunctionMaps mapping = {.updaters = updaters,
                                .renderers = renderers,
                                .handlers = handlers};