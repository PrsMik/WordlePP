#include "AssetManager.hpp"
#include "SDL3_ttf/SDL_ttf.h"

AssetManager::AssetManager(SDL_Renderer *_renderer) : renderer(_renderer)
{
}

AssetManager::~AssetManager()
{
    for (auto const &[key, font] : fonts)
    {
        if (font != nullptr)
        {
            TTF_CloseFont(font);
        }
    }
    fonts.clear();
}

void AssetManager::loadFonts(const std::string &name, const std::string &fontPath, int ptSize)
{
    if (fonts.contains(name))
    {
        return;
    }

    TTF_Font *font = TTF_OpenFont(fontPath.c_str(), ptSize);
    if (font == nullptr)
    {
        return;
    }

    fonts[name] = font;
}

void AssetManager::setFontSize(const std::string &name, float size)
{
    TTF_SetFontSize(fonts.at(name), size);
}

const TTF_Font *AssetManager::getFont(const std::string &name) const
{
    if (fonts.contains(name))
    {
        return fonts.at(name);
    }
    return nullptr;
}

void AssetManager::loadTextures(const std::string &texturePath)
{
}

SDL_Texture *AssetManager::getTexture(const std::string &name)
{
    return nullptr;
}