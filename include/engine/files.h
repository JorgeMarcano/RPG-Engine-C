#ifndef FILES_H
#define FILES_H

#include "globals.h"

// Handle file loading and saving

void files_load_texture(SDL_Renderer* renderer, const char* src, globals_Texture** dest);
void files_clean_texture(globals_Texture* texture);

// startTileset is head of a LinkedList (not an actual tileset)
bool files_load_tileset(const char* src, globals_Tileset** dest, SDL_Renderer* renderer, globals_Tileset* startTileset);
void files_clean_tileset(globals_Tileset* tileset);

// Remove 1 usage from count, if 0 usages left, clean this texture and return true
bool files_remove_usage(globals_Tileset* tileset);

#endif