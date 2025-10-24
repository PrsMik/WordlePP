#pragma once

#include "CharBox.hpp"
#include "SDL3_ttf/SDL_ttf.h"
#include <string>
#include <vector>

class WordRow
{
private:
    float startX, startY;
    float boxWidth;
    float boxHeight;
    float spacing;
    int wordLength;
    const TTF_Font *font;
    SDL_Renderer *renderer;
    TTF_TextEngine *engine;
    std::vector<CharBox> boxes;

public:
    WordRow(float startX, float startY, float boxWidth,
            float boxHeight, float spacing, int length,
            const TTF_Font *font, SDL_Renderer *renderer, TTF_TextEngine *engine);

    void setWord(const std::string &word, CharStatus defaultStatus);

    void setStatuses(const std::string &word, const std::vector<CharStatus> &statuses);

    void render();
};