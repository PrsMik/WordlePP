#pragma once

#include "CharBox.hpp"
#include <map>
#include <string>
#include <vector>

class KeyboardDisplay
{
private:
    std::map<std::string, CharBox> keyBoxes;
    std::map<std::string, CharStatus> keyStatuses;
    SDL_Renderer *renderer;
    const TTF_Font *font;

    std::vector<std::string> rows;

    float startX;
    float startY;
    float keyWidth, keyHeight;
    float rowSpacing, keySpacing;
    float width, height;

public:
    KeyboardDisplay(float startX, float startY, float keyWidth, float keyHeight,
                    float rowSpacing, float keySpacing,
                    const TTF_Font *font, SDL_Renderer *renderer,
                    const std::string &alphabet);

    void updateStatus(std::string key, CharStatus status);
    void render();

    float getWidth() const { return width; };

    float getHeight() const { return height; };
};