#include "Engine/files.h"

#include "string.h"

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
    if (f == NULL)
        return false;

    char buff[256];
    buff[0] = 0;
    char temp_buff[256];
    temp_buff[0] = 0;

    globals_Tileset* tileset = malloc(sizeof(globals_Tileset));

    int hasMatched = false;
    while (!hasMatched && (fscanf(f, "%s", buff) != EOF)) {
        hasMatched = (strcmp(buff, "<tileset") == 0);
    }

    if (!hasMatched) {
        printf("Couldn't find tileset tag!\n");
        fclose(f);
        free(tileset);
        return false;
    }

    size_t buff_len;
    int tilecount = -1;
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
    hasMatched = false;
    while (!hasMatched && (fscanf(f, "%s", buff) != EOF)) {
        hasMatched = (strcmp(buff, "<image") == 0);
    }

    if (!hasMatched) {
        printf("Couldn't find image tag!\n");
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