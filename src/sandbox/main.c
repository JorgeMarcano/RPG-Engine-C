#include <stdio.h>
#include <stdlib.h>

#include "Engine/engine.h"

// FIXME: Temp functions
Scene* createScene(Engine* engine);
Sprite* createSprite(Engine* engine);

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

    // TEMP stuff
    Scene* scene = createScene(engine);
    Sprite* sprite = createSprite(engine);
    scene->size_sprites = 1;
    scene->sprites = malloc(scene->size_sprites * sizeof(Sprite*));
    scene->sprites[0] = sprite;
    
    engine_start(engine, scene, "res/tilesets/tiles-map.tsx");

    // scene_destroy(scene, true);
    engine_clean(engine);

	return 0;
}

Scene* createScene(Engine* engine) {
    Scene* scene = malloc(sizeof(Scene));
    scene->center.x = 0;
    scene->center.y = 0;
    scene->size_sprites = 0;
    Hud* hud = malloc(sizeof(Hud));
    hud->updater = &entity_default_updater;
    hud->renderer = &entity_default_renderer;
    hud->handler = &entity_default_handler;
    scene->hud = hud;
    Map* map = malloc(sizeof(Map));
    map->updater = &entity_default_updater;
    map->renderer = &map_default_renderer;
    map->handler = &entity_default_handler;
    map->size_px.h = 1600;
    map->size_px.w = 1600;
    globals_Texture* texture;
    files_load_texture(engine->renderer, "res/bg.bmp", &texture);
    map->texture = texture->texture;
    free(texture);
    scene->map = map;
    scene->renderer = engine->renderer;

    return scene;
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