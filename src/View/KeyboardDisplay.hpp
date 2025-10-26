#pragma once

#include "../Model/GameState.hpp"
#include "CharBox.hpp"
#include "GameLayout.hpp"
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
    std::map<std::string, GameStateDTO::LetterStatus> c_cachedKeyStatuses;

    void updateStatus(std::string key, CharStatus status);

public:
    KeyboardDisplay(SDL_Renderer *renderer, TTF_TextEngine *engine, const LayoutMetrics &metrics,
                    const TTF_Font *font, const GameStateDTO &state);

    void render(const std::map<std::string, GameStateDTO::LetterStatus> &newKeyStatuses);

    float getWidth() const { return width; };
    float getHeight() const { return height; };
};