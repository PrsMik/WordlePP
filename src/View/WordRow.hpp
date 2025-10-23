#pragma once

#include "CharBox.hpp"
#include <string>
#include <vector>

class WordRow
{
private:
    std::vector<CharBox> boxes;
    float startX, startY;
    float boxWidth;
    float boxHeight;
    float spacing;
    const TTF_Font *font;
    SDL_Renderer *renderer;
    int wordLength;

public:
    WordRow(float startX, float startY, float boxWidth, float boxHeight, float spacing, int length, const TTF_Font *font, SDL_Renderer *renderer);

    void setWord(const std::string &word, CharStatus defaultStatus);

    void setStatuses(const std::string &word, const std::vector<CharStatus> &statuses);

    void render();
};