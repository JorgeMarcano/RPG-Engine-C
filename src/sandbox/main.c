#include <stdio.h>
#include <stdlib.h>

#include "Engine/engine.h"

// FIXME: Temp functions
Scene* createScene(Engine* engine);
Sprite* createSprite(Engine* engine);

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

    engine_init(&desc, &engine);
    
    engine_start(engine, "res/maps/test.tmx");

    engine_clean(engine);

	return 0;
}

Sprite* createSprite(Engine* engine) {
    Sprite* sprite = malloc(sizeof(Sprite));

    sprite->updater = &entity_default_updater;
    sprite->renderer = &sprite_default_renderer;
    sprite->handler = &entity_default_handler;

    files_load_texture(engine->renderer, "res/test.bmp", &(sprite->texture));

    sprite->pos_px.x = (SCREEN_WIDTH - sprite->texture->size.w) / 2;
    sprite->pos_px.y = (SCREEN_HEIGHT - sprite->texture->size.h) / 2;

    return sprite;
}

SDL_Point speed;

void update_map(void* entity, void* scene, Uint32 deltaT) {
    static Uint32 lastUpdate = 0;
    lastUpdate += deltaT;

    Scene* p_scene = scene;

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
}

void event_map(void* entity, void* scene, SDL_Event* event) {
    Scene* p_scene = scene;

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
const FunctionMaps mapping = {.updaters = updaters, .renderers = renderers, .handlers = handlers};