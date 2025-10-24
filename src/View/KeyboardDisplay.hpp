#pragma once

#include "CharBox.hpp"
#include "SDL3_ttf/SDL_ttf.h"
#include <map>
#include <string>
#include <vector>

class KeyboardDisplay
{
private:
    float startX;
    float startY;
    float keyWidth, keyHeight;
    float rowSpacing, keySpacing;
    const TTF_Font *font;
    SDL_Renderer *renderer;
    TTF_TextEngine *engine;

    float width, height;
    std::vector<std::string> keyRows;
    std::map<std::string, CharBox> keyBoxes;
    std::map<std::string, CharStatus> keyStatuses;

public:
    KeyboardDisplay(float startX, float startY, float keyWidth, float keyHeight,
                    float rowSpacing, float keySpacing,
                    const TTF_Font *font, SDL_Renderer *renderer,
                    const std::string &alphabet, TTF_TextEngine *engine);

    void updateStatus(std::string key, CharStatus status);
    void render();

    float getWidth() const { return width; };

    float getHeight() const { return height; };
};