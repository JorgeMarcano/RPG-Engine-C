#include "Engine/scene.h"
#include "Engine/engine.h"

void scene_update_render(Scene* scene, Uint32 deltaT) {
    if (scene->map) {
        // Update map
        (*scene->map->updater)(scene->map, scene, deltaT);
        // Render map
        (*scene->map->renderer)(scene->map, scene);
    }

    // For every sprite
    for (size i = 0; i < scene->size_sprites; i++) {
        // Update sprite
        (*scene->sprites[i]->updater)(scene->sprites[i], scene, deltaT);
        // Render sprite
        (*scene->sprites[i]->renderer)(scene->sprites[i], scene);
    }

    if (scene->hud) {
        // Update HUD
        (*scene->hud->updater)(scene->hud, scene, deltaT);
        // Render HUD
        (*scene->hud->renderer)(scene->hud, scene);
    }
}

void scene_update(Scene* scene, Uint32 deltaT) {
    // Update map
    if (scene->map)
        (*scene->map->updater)(scene->map, scene, deltaT);

    // For every sprite
    for (size i = 0; i < scene->size_sprites; i++) {
        // Update sprite
        (*scene->sprites[i]->updater)(scene->sprites[i], scene, deltaT);
    }

    // Update HUD
    if (scene->hud)
        (*scene->hud->updater)(scene->hud, scene, deltaT);
}

void scene_handle_event(Scene* scene, SDL_Event* event) {
    // Map handle event
    if (scene->map)
        (*scene->map->handler)(scene->map, scene, event);

    // For every sprite, handle event
    for (size i = 0; i < scene->size_sprites; i++)
        (*scene->sprites[i]->handler)(scene->sprites[i], scene, event);

    // HUD handle event
    if (scene->hud)
        (*scene->hud->handler)(scene->hud, scene, event);
}

bool scene_save();

void scene_destroy(Scene* scene, bool destroyHUD) {
    if (scene == NULL)
        return;

    map_destroy(scene->map);
    scene->map = NULL;
    
    for (size i = 0; i < scene->size_sprites; i++) {
        sprite_destroy(scene->sprites[i]);
        scene->sprites[i] = NULL;
    }
    free(scene->sprites);
    scene->sprites = NULL;

    if (destroyHUD)
        hud_destroy(scene->hud);
    scene->hud = NULL;

    free(scene);
}

void scene_change(Scene* scene, Door* door, bool keep_hud) {
    SDL_Event event;
    SDL_memset(&event, 0, sizeof(SDL_Event));

    event.type = scene_event_id;
    event.user.code = keep_hud ? SCENE_EVENT_CHANGE_HUD_KEEP : SCENE_EVENT_CHANGE_HUD_REMOVE;
    event.user.data1 = scene;
    event.user.data2 = door;

    SDL_PushEvent(&event);
    printf("Requested a Scene change!\n");
}