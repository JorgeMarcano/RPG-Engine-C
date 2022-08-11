#ifndef ENTITY_H
#define ENTITY_H

#include "globals.h"

// Entities include: Sprites, Maps, HUDs
// Ideally, everything should be contained within a scene (no inter-scene communication)
// IF inter-scene communication must be done, it must be event-like driven

typedef struct Door {
    char* scene_src;
    int spawnID;

    SDL_Point location;

    struct Door* next;
} Door;

typedef struct Sprite {
    globals_Texture* texture;
    SDL_Point pos_tile;
    SDL_Point pos_px;
    SDL_Point speed;

    globals_Tile animation;
    SDL_Point animation_size;

    Updater updater;
    Renderer renderer;
    Handler handler;

    unsigned int spriteID;
} Sprite;

typedef struct Map {
    SDL_Texture* texture;
    SDL_Rect size_px;
    SDL_Rect size_tile;

    // 2D array of size size_tile.w by size_tile.h
    bool** collisionTiles;
    // Linked List
    Door* doors;

    Updater updater;
    Renderer renderer;
    Handler handler;
} Map;

typedef struct Hud {
    globals_Texture* texture;

    Updater updater;
    Renderer renderer;
    Handler handler;
} Hud;

void map_default_renderer(void* entity, void* scene);
void sprite_default_renderer(void* entity, void* scene);

void entity_default_updater(void* entity, void* scene, Uint32 deltaT);
void entity_default_renderer(void* entity, void* scene);
void entity_default_handler(void* entity, void* scene, SDL_Event* event);

bool entity_detect_collision(Sprite* sprite, struct Scene* scene, Sprite** collider);

void map_destroy(Map* map);
void sprite_destroy(Sprite* sprite);
void hud_destroy(Hud* hud);

void map_generate(SDL_Renderer* renderer, globals_Tile** tiling, bool** collisions, SDL_Rect map_size, Map** dest);

bool entity_check_door(SDL_Point position, struct Scene* scene, Door** dest);

#endif