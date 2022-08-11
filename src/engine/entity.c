#include "Engine/entity.h"
#include "Engine/scene.h"
#include "Engine/engine.h"

void map_default_renderer(void* entity, void* scene) {
    Scene* p_scene = (Scene*) scene;
    Map* p_map = (Map*) entity;

        // SDL_RenderCopy(p_scene->renderer, p_map->texture, NULL, NULL);
        // return;

    // Get the Rect to display
    SDL_Point center_pos_px;
    center_pos_px.x = p_scene->center.x * tile_size.w + tile_size.w / 2;
    center_pos_px.y = p_scene->center.y * tile_size.h + tile_size.h / 2;
    SDL_Rect src_rect = scene_size;
    SDL_Rect dst_rect = scene_size;
    
    src_rect.x = center_pos_px.x - (scene_size.w / 2);
    if (src_rect.x < 0) {
        dst_rect.x = -src_rect.x;
        src_rect.w += src_rect.x;
        dst_rect.w = src_rect.w;
        src_rect.x = 0;
    }
    if (src_rect.x + src_rect.w > p_map->size_px.w) {
        src_rect.w = p_map->size_px.w - src_rect.x;
        dst_rect.w = src_rect.w;
    }

    src_rect.y = center_pos_px.y - (scene_size.h / 2);
    if (src_rect.y < 0) {
        dst_rect.y = -src_rect.y;
        src_rect.h += src_rect.y;
        dst_rect.h = src_rect.h;
        src_rect.y = 0;
    }
    if (src_rect.y + src_rect.h > p_map->size_px.h) {
        src_rect.h = p_map->size_px.h - src_rect.y;
        dst_rect.h = src_rect.h;
    }
    
    engine_render(p_scene->renderer, p_map->texture, &src_rect, &dst_rect, p_scene->scale);
}

void sprite_default_renderer(void* entity, void* scene) {
    Sprite* p_sprite = (Sprite*) entity;
    Scene* p_scene = (Scene*) scene;

    SDL_Rect pos = p_sprite->texture->size;
    pos.x = p_sprite->pos_px.x;
    pos.y = p_sprite->pos_px.y;
    
    engine_render(p_scene->renderer, p_sprite->texture->texture, NULL, &pos, p_scene->scale);
}

// Entity Defaults do nothing
void entity_default_updater(void* entity, void* scene, Uint32 deltaT) {
    return;
}

void entity_default_renderer(void* entity, void* scene) {
    return;
}

void entity_default_handler(void* entity, void* scene, SDL_Event* event) {
    return;
}

void map_destroy(Map* map) {
    if (map == NULL)
        return;

    Door* currNode = map->doors;
    Door* tempNode;
    while(currNode != NULL) {
        tempNode = currNode->next;
        free(currNode);
        currNode = tempNode;
    }

    SDL_DestroyTexture(map->texture);
    map->texture = NULL;

    if (map->collisionTiles) {
        for (int i = 0; i < map->size_tile.w; i++) {
            free(map->collisionTiles[i]);
            map->collisionTiles[i] = NULL;
        }

        free(map->collisionTiles);
    }

    free(map);
}

void sprite_destroy(Sprite* sprite) {
    if (sprite == NULL)
        return;

    SDL_DestroyTexture(sprite->texture->texture);
    free(sprite->texture);

    free(sprite);
}
void hud_destroy(Hud* hud) {
    if (hud == NULL)
        return;

    SDL_DestroyTexture(hud->texture->texture);
    free(hud->texture);

    free(hud);
}

void map_generate(SDL_Renderer* renderer, globals_Tile** tiling, bool** collisions, SDL_Rect map_size, Map** dest) {
    Map* map = malloc(sizeof(Map));

    (*dest) = map;

    map->collisionTiles = collisions;
    map->size_tile = map_size;
    map->size_px.w = map_size.w * tile_size.w;
    map->size_px.h = map_size.h * tile_size.h;

    map->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, map->size_px.w, map->size_px.h);
    if (tiling == NULL)
        return;

    SDL_SetRenderTarget(renderer, map->texture);

    SDL_Rect src_rect = tile_size;
    SDL_Rect dst_rect = tile_size;
    int j;
    for (int i = 0; i < map_size.w; i++) {
        for (j = 0; j < map_size.h; j++) {
            globals_Tile* tile = &(tiling[i][j]);

            if (tile->tileset == NULL)
                continue;

            src_rect.x = tile->pos.x * tile_size.w;
            src_rect.y = tile->pos.y * tile_size.h;

            dst_rect.x = i * tile_size.w;
            dst_rect.y = j * tile_size.h;

            SDL_RenderCopy(renderer, tile->tileset->texture->texture, &src_rect, &dst_rect);
        }
    }

    SDL_SetRenderTarget(renderer, NULL);
}

bool entity_detect_collision(Sprite* sprite, Scene* scene, Sprite** collider) {
    SDL_Point position = sprite->pos_tile;
    (*collider) = NULL;
    if (position.x < 0 || position.x > scene->map->size_tile.w)
        return false;

    if (position.y < 0 || position.y > scene->map->size_tile.h)
        return false;

    if (scene->map->collisionTiles[position.x][position.y])
        return true;

    // Sprite collisions
    SDL_Point* tempPoint;
    for (size i = 0; i < scene->size_sprites; i++) {
        tempPoint = &(scene->sprites[i]->pos_tile);
        if (position.x == tempPoint->x && position.y == tempPoint->y) {
            if (sprite->spriteID != scene->sprites[i]->spriteID) {
                (*collider) = scene->sprites[i];
                return true;
            }
        }
    }

    return false;
}

bool entity_check_door(SDL_Point position, Scene* scene, Door** dest) {
    Door* door = scene->map->doors;

    (*dest) = NULL;
    while (door != NULL) {
        if (position.x == door->location.x && position.y == door->location.y) {
            (*dest) = door;

            return true;
        }

        door = door->next;
    }

    return false;
}