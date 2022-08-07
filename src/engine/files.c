#include "Engine/files.h"
#include "Engine/scene.h"

#include "string.h"

typedef struct Scene_Tileset {
    globals_Tileset* tileset;
    int firstgid;
    struct Scene_Tileset* next;
} Scene_Tileset;

void files_load_texture(SDL_Renderer* renderer, const char* src, globals_Texture** dest) {
    SDL_Surface* surface;

    surface = SDL_LoadBMP(src);
    if (surface == NULL) {
        printf("Unable to load image %s! SDL Error: %s\n", src, SDL_GetError());
        (*dest) = NULL;
        return;
    }

    (*dest) = malloc(sizeof(globals_Texture));
    (*dest)->texture = SDL_CreateTextureFromSurface(renderer, surface);
    if ((*dest)->texture == NULL) {
        printf("Failed to create texture from %s! SDL Error: %s\n", src, SDL_GetError());
        free(*dest);
        (*dest) = NULL;
    }
    else {
        (*dest)->size.x = 0;
        (*dest)->size.y = 0;
        (*dest)->size.w = surface->w;
        (*dest)->size.h = surface->h;
    }

    SDL_FreeSurface(surface);

    return;
}

void files_clean_texture(globals_Texture* texture) {
    if (texture) {
        if (texture->texture) {
            SDL_DestroyTexture(texture->texture);
            texture->texture = NULL;
        }

        free(texture);
    }
}

bool tagcmp(const char* str1, const char* str2) {
    int i = 0;
    for (;str1[i] != '=' && str2[i] != '='; i++)
        if (str1[i] != str2[i])
            return false;

    return (str1[i] == str2[i]);
}

bool tagfind(FILE* file, const char* tag, char* buff, bool print) {
    int hasMatched = false;
    while (!hasMatched && (fscanf(file, "%s", buff) != EOF)) {
        hasMatched = (strcmp(buff, tag) == 0);
    }

    if (!hasMatched) {
        if (print)
            printf("Couldn't find %s tag!\n", tag);
        return false;
    }

    return true;
}

// Loads tile from TSX file
bool files_load_tileset(const char* src, globals_Tileset** dest, SDL_Renderer* renderer, globals_Tileset* startTileset) {
    // Make sure the tileset isn't already in use
    globals_Tileset* currNode = startTileset->next;
    while(currNode != NULL) {
        if (strcmp(currNode->src_name, src)) {
            // Already exists
            currNode->usages++;
            (*dest) = currNode;
            return true;
        }

        currNode = currNode->next;
    }

    FILE* f = fopen(src, "r");
    if (f == NULL) {
        printf("Failed to open file: %s!\n", src);
        return false;
    }

    char buff[256];
    buff[0] = 0;
    char temp_buff[256];
    temp_buff[0] = 0;

    globals_Tileset* tileset = malloc(sizeof(globals_Tileset));

    if (!tagfind(f, "<tileset", buff, true)) {
        fclose(f);
        free(tileset);
        return false;
    }

    size_t buff_len;
    int tilecount = -1;
    int hasMatched = false;
    do {
        hasMatched = (fscanf(f, "%s", buff) == EOF);
        buff_len = strlen(buff);

        if (tagcmp(buff, "tilecount=")) {
            sscanf(buff, "tilecount=\"%d", &tilecount);

        } else if (tagcmp(buff, "columns=")) {
            sscanf(buff, "columns=\"%d", &(tileset->size_tiles.w));

        } else if (tagcmp(buff, "name=")) {
            sscanf(buff, "name=\"%s", temp_buff);
            for (size_t i = 0; temp_buff[i] != '\0'; i++) {
                if (temp_buff[i] == '\"') {
                    temp_buff[i] = '\0';
                    break;
                }
            }

            size_t name_len = strlen(temp_buff) + 1; // Add 1 for '\0'
            tileset->set_name = malloc(name_len * sizeof(char));
            strcpy(tileset->set_name, temp_buff);
        }

    } while (!hasMatched && buff_len && buff[buff_len-1] != '>');

    if (hasMatched) {
        printf("Reached EOF without closing tileset tag!\n");
        fclose(f);
        files_clean_tileset(tileset);
        return false;
    }

    if (tilecount == -1) {
        printf("Did not find \"tilecount\" tag!\n");
        fclose(f);
        files_clean_tileset(tileset);
        return false;
    }

    tileset->size_tiles.h = tilecount / tileset->size_tiles.w;

    // Get image source
    if (!tagfind(f, "<image", buff, true)) {
        fclose(f);
        free(tileset);
        return false;
    }

    temp_buff[0] = '\0';
    do {
        hasMatched = (fscanf(f, "%s", buff) == EOF);
        buff_len = strlen(buff);

        if (tagcmp(buff, "source=")) {
            sscanf(buff, "source=\"%s", temp_buff);
            for (size_t i = 0; temp_buff[i] != '\0'; i++) {
                if (temp_buff[i] == '\"') {
                    temp_buff[i] = '\0';
                    break;
                }
            }

            // FIXME: must make this path relative to game and not to .tsx file (res/tileset/)
            const char* rel_path = "res/tilesets/";
            size_t i = strlen(temp_buff) + 1;
            for (; i > 0; i--)
                temp_buff[i + 13] = temp_buff[i];

            temp_buff[13] = temp_buff[0];

            for (i = 0; i < 13; i++)
                temp_buff[i] = rel_path[i];
            // end of FIXME, should improve this? less hardcoded maybe?

            files_load_texture(renderer, temp_buff, &(tileset->texture));
        }

    } while (!hasMatched && buff_len && buff[buff_len-1] != '>');

    if (hasMatched) {
        printf("Reached EOF without closing image tag!\n");
        fclose(f);
        files_clean_tileset(tileset);
        return false;
    }

    if (tileset->texture == NULL) {
        printf("Failed to load image from src:\"%s\"!", temp_buff);
    }

    tileset->usages = 1;
    tileset->next = startTileset->next;
    tileset->prev = startTileset;
    startTileset->next = tileset;
    if (tileset->next)
        tileset->next->prev = tileset;
    
    fclose(f);
    (*dest) = tileset;
    return true;
}

void files_clean_tileset(globals_Tileset* tileset) {
    if (tileset->prev != NULL)
        tileset->prev->next = tileset->next;
    if (tileset->next != NULL)
        tileset->next->prev = tileset->prev;

    files_clean_texture(tileset->texture);
    tileset->texture = NULL;

    if (tileset->set_name)
        free(tileset->set_name);

    free(tileset);
}

bool files_remove_usage(globals_Tileset* tileset) {
    if (tileset->usages < 2) {
        files_clean_tileset(tileset);
        return true;
    }

    tileset->usages--;
    return false;
}

void get_tile(int tileID, Scene_Tileset* tilesets, globals_Tile* dest) {
    Scene_Tileset* currNode = tilesets;

    dest->tileset = NULL;

    while (currNode != NULL) {
        // Is the tileID within this tileset
        if (tileID >= tilesets->firstgid) {
            int lastgid = tilesets->firstgid;
            lastgid += (tilesets->tileset->size_tiles.w * tilesets->tileset->size_tiles.h);
            if (tileID < lastgid) {
                dest->tileset = tilesets->tileset;

                int offset = tileID - tilesets->firstgid;
                dest->pos.x = offset % tilesets->tileset->size_tiles.w;
                dest->pos.y = offset / tilesets->tileset->size_tiles.w;

                return;
            }
        }

        currNode = currNode->next;
    }
}

bool files_load_scene(const char* src, Scene** dest, SDL_Renderer* renderer, globals_Tileset* startTileset) {
    FILE* f = fopen(src, "r");
    if (f == NULL) {
        printf("Failed to open file: %s!\n", src);
        return false;
    }

    char buff[256];
    buff[0] = 0;
    char temp_buff[256];
    temp_buff[0] = 0;

    Scene* scene = malloc(sizeof(Scene));
    scene->size_sprites = 0;

    if (!tagfind(f, "<map", buff, true)) {
        fclose(f);
        free(scene);
        return false;
    }

    size_t buff_len;
    bool isEOF = false;
    SDL_Rect map_size = {0, 0, 0, 0};
    do {
        isEOF = (fscanf(f, "%s", buff) == EOF);
        buff_len = strlen(buff);

        if (tagcmp(buff, "width=")) {
            sscanf(buff, "width=\"%d", &(map_size.w));

        } else if (tagcmp(buff, "height=")) {
            sscanf(buff, "height=\"%d", &(map_size.h));

        }

    } while (!isEOF && buff_len && buff[buff_len-1] != '>');

    if (isEOF) {
        printf("Reached EOF without closing map tag!\n");
        fclose(f);
        scene_destroy(scene, true);
        return false;
    }

    // Get all the tilesets for this scene
    Scene_Tileset* baseNode = NULL;
    Scene_Tileset* tempNode = NULL;

    bool isLayer = false;
    while (!isLayer && (fscanf(f, "%s", buff) != EOF)) {
        if (strcmp(buff, "<tileset") == 0) {
            tempNode = malloc(sizeof(Scene_Tileset));
            tempNode->next = baseNode;
            baseNode = tempNode;

            isEOF = false;
            do {
                isEOF = (fscanf(f, "%s", buff) == EOF);
                buff_len = strlen(buff);

                if (tagcmp(buff, "firstgid=")) {
                    sscanf(buff, "firstgid=\"%d", &(tempNode->firstgid));

                } else if (tagcmp(buff, "source=")) {
                    sscanf(buff, "source=\"%s", temp_buff);
                    for (size_t i = 0; temp_buff[i] != '\0'; i++) {
                        if (temp_buff[i] == '\"') {
                            temp_buff[i] = '\0';
                            break;
                        }
                    }

                    // FIXME: must make this path relative to game and not to .tmx file (res/maps/)
                    const char* rel_path = "res/maps/";
                    size_t i = strlen(temp_buff) + 1;
                    for (; i > 0; i--)
                        temp_buff[i + 9] = temp_buff[i];

                    temp_buff[9] = temp_buff[0];

                    for (i = 0; i < 9; i++)
                        temp_buff[i] = rel_path[i];
                    // end of FIXME, should improve this? less hardcoded maybe?

                    files_load_tileset(temp_buff, &(tempNode->tileset), renderer, startTileset);
                }

            } while (!isEOF && buff_len && buff[buff_len-1] != '>');
        } else {
            isLayer = (strcmp(buff, "<layer") == 0);
        }
    }

    if (baseNode == NULL) {
        printf("Need at least 1 tileset!\n");
        fclose(f);
        scene_destroy(scene, true);
        return false;
    }

    if (!isLayer) {
        printf("Failed to find layer tag!\n");
        while(baseNode != NULL) {
            tempNode = baseNode->next;
            free(baseNode);
            baseNode = tempNode;
        }
        fclose(f);
        scene_destroy(scene, true);
        return false;
    }

    // Get the layers
    bool** collisions = malloc(map_size.w * sizeof(bool*));
    globals_Tile** tiling = malloc(map_size.w * sizeof(globals_Tile*));
    for (int i = 0; i < map_size.w; i++) {
        collisions[i] = malloc(map_size.h * sizeof(bool));
        tiling[i] = malloc(map_size.h * sizeof(globals_Tile));
    }
    bool hasFailed = false;
    const char* delim = ",";
    char* tok;
    do {
        // Read entire layer
        // Get layer name
        isEOF = false;
        temp_buff[0] = '\0';
        do {
            isEOF = (fscanf(f, "%s", buff) == EOF);
            buff_len = strlen(buff);

            if (tagcmp(buff, "name=")) {
                sscanf(buff, "name=\"%s", temp_buff);
                for (size_t i = 0; temp_buff[i] != '\0'; i++) {
                    if (temp_buff[i] == '\"') {
                        temp_buff[i] = '\0';
                        break;
                    }
                }
            }

        } while (!isEOF && buff_len && buff[buff_len-1] != '>');

        if (strcmp(temp_buff, "Map") == 0) {
            isEOF = !tagfind(f, "<data", buff, true);
            do {
                isEOF = (fscanf(f, "%s", buff) == EOF);
                buff_len = strlen(buff);
            } while (!isEOF && buff_len && buff[buff_len-1] != '>');

            if (!isEOF) {
                // For each line
                int i;
                int tileID;
                for (int j = 0; j < map_size.h; j++) {
                    fscanf(f, "%s", buff);
                    
                    // Get first token
                    tok = strtok(buff, delim);
                    
                    sscanf(tok, "%d", &tileID);
                    get_tile(tileID, baseNode, &(tiling[0][j]));

                    // Loop for the rest of the tokens
                    for (i = 1; i < map_size.w; i++) {
                        tok = strtok(NULL, delim);
                        
                        sscanf(tok, "%d", &tileID);
                        get_tile(tileID, baseNode, &(tiling[i][j]));
                    }
                }

                map_generate(renderer, tiling, NULL, map_size, &(scene->map));

                // TODO: add handlers
                scene->map->handler = &entity_default_handler;
                scene->map->renderer = &map_default_renderer;
                scene->map->updater = &entity_default_updater;
            }

        } else if (strcmp(temp_buff, "Collisions") == 0) {
            isEOF = !tagfind(f, "<data", buff, true);
            do {
                isEOF = (fscanf(f, "%s", buff) == EOF);
                buff_len = strlen(buff);
            } while (!isEOF && buff_len && buff[buff_len-1] != '>');

            if (!isEOF) {
                // For each line
                int i;
                int tileID;
                for (int j = 0; j < map_size.h; j++) {
                    fscanf(f, "%s", buff);
                    
                    // Get first token
                    tok = strtok(buff, delim);
                    
                    sscanf(tok, "%d", &tileID);
                    collisions[0][j] = (tileID != 0);

                    // Loop for the rest of the tokens
                    for (i = 1; i < map_size.w; i++) {
                        tok = strtok(NULL, delim);
                        
                        sscanf(tok, "%d", &tileID);
                        collisions[i][j] = (tileID != 0);
                    }
                }
            }
        } else if (strcmp(temp_buff, "Sprites") == 0) {
            //TODO:
        } else if (strcmp(temp_buff, "Doors") == 0) {
            //TODO:
        } else if (strcmp(temp_buff, "Spawns") == 0) {
            //TODO:
            scene->center.x = 0;
            scene->center.y = 0;
        } else {
            printf("Layer name \"%s\" not a valid name, ignoring this layer!\n", temp_buff);
        }

        if (hasFailed || isEOF) {
            for (int i = 0; i < map_size.w; i++) {
                free(tiling[i]);
                free(collisions[i]);
            }
            free(tiling);
            free(collisions);

            printf("Failed while parsing layers!\n");
            while(baseNode != NULL) {
                tempNode = baseNode->next;
                free(baseNode);
                baseNode = tempNode;
            }
            fclose(f);
            scene_destroy(scene, true);
            return false;
        }

    } while (tagfind(f, "<layer", buff, false));

    for (int i = 0; i < map_size.w; i++)
        free(tiling[i]);
    free(tiling);

    if (scene->map == NULL)
        map_generate(renderer, NULL, NULL, map_size, &(scene->map));

    scene->map->collisionTiles = collisions;

    printf("Finished loading scene!\n");

    while(baseNode != NULL) {
        tempNode = baseNode->next;
        free(baseNode);
        baseNode = tempNode;
    }

    scene->renderer = renderer;
    (*dest) = scene;
    fclose(f);
    return true;
}