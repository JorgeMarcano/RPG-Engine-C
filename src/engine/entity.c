#include "Engine/entity.h"
#include "Engine/scene.h"

void map_default_renderer(void* entity, void* scene) {
    Scene* p_scene = (Scene*) scene;
    Map* p_map = (Map*) entity;

    // Get the Rect to display
    SDL_Point center_pos_px;
    center_pos_px.x = p_scene->center.x * tile_size.w + tile_size.w / 2;
    center_pos_px.y = p_scene->center.y * tile_size.h + tile_size.h / 2;
    SDL_Rect src_rect = screen_size;
    SDL_Rect dst_rect = screen_size;
    
    src_rect.x = center_pos_px.x - (screen_size.w / 2);
    if (src_rect.x < 0) {
        dst_rect.x = -src_rect.x;
        src_rect.w += src_rect.x;
        dst_rect.w = src_rect.w;
        src_rect.x = 0;
    }
    if (src_rect.x + src_rect.w > p_map->size_px.w) {
        src_rect.w = src_rect.x - p_map->size_px.w;
        dst_rect.w = src_rect.w;
    }

    src_rect.y = center_pos_px.y - (screen_size.h / 2);
    if (src_rect.y < 0) {
        dst_rect.y = -src_rect.y;
        src_rect.h += src_rect.y;
        dst_rect.h = src_rect.h;
        src_rect.y = 0;
    }
    if (src_rect.y + src_rect.h > p_map->size_px.h) {
        src_rect.h = src_rect.y - p_map->size_px.h;
        dst_rect.h = src_rect.h;
    }
    
    SDL_RenderCopy(p_scene->renderer, p_map->texture, &src_rect, &dst_rect);
}

void sprite_default_renderer(void* entity, void* scene) {
    Sprite* p_sprite = (Sprite*) entity;
    Scene* p_scene = (Scene*) scene;

    SDL_Rect pos = p_sprite->texture->size;
    pos.x = p_sprite->pos_px.x;
    pos.y = p_sprite->pos_px.y;
    
    SDL_RenderCopy(p_scene->renderer, p_sprite->texture->texture, NULL, &pos);
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
    SDL_DestroyTexture(map->texture);
    map->texture = NULL;

    free(map);
}

void sprite_destroy(Sprite* sprite) {
    SDL_DestroyTexture(sprite->texture->texture);
    free(sprite->texture);

    free(sprite);
}
void hud_destroy(Hud* hud) {
    SDL_DestroyTexture(hud->texture->texture);
    free(hud->texture);

    free(hud);
}