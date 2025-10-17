#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <string>

class AssetManager
{
private:
    SDL_Renderer *renderer;

public:
    AssetManager(SDL_Renderer *renderer);

    void loadFonts(const std::string &fontPath);

    void loadTextures(const std::string &texturePath);

    TTF_Font *getFont(const std::string &name);

    SDL_Texture *getTexture(const std::string &name);
};