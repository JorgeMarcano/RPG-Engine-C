#ifndef SCENE_H
#define SCENE_H

#include "globals.h"
#include "entity.h"
#include "files.h"

// A Scene is a structure that contains the information of:
//      - All sprites present in the scene, whether visible or off-screne
//      - The map of this scene
//      - The HUD
//      - A pointer to the renderer
//      - Current camera location

typedef struct Scene{
    Sprite**  sprites;
    size    size_sprites;
    Map*   map;
    Hud*   hud;

    SDL_Renderer* renderer;

    SDL_Point center;   // Which tile is in the center of the screen
} Scene;

// Update and Render Scene (if scene is visible)
void scene_update_render(Scene* scene, Uint32 deltaT);

// Update Scene only (if scene is not visible), might be used if updates must happen for different scene than present
void scene_update(Scene* scene, Uint32 deltaT);

// Let scene's components handle event
void scene_handle_event(Scene* scene, SDL_Event* event);

bool scene_load(const char* src, struct Engine* engine, Scene** dest); // FIXME: Replace scene input with file input
bool scene_save();

void scene_destroy(Scene* scene, bool destroyHUD);

#endif