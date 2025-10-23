#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <map>
#include <string>

class AssetManager
{
private:
    SDL_Renderer *renderer;
    std::map<std::string, TTF_Font *> fonts;

public:
    AssetManager(SDL_Renderer *renderer);

    void loadFonts(const std::string &name, const std::string &fontPath, int ptSize);

    void loadTextures(const std::string &texturePath);

    void setFontSize(const std::string &name, float size);

    const TTF_Font *getFont(const std::string &name) const;

    SDL_Texture *getTexture(const std::string &name);

    ~AssetManager();
};