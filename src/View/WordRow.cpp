#include "WordRow.hpp"
#include "Utils.hpp"
#include <SDL3/SDL_stdinc.h>
#include <vector>

WordRow::WordRow(float _startX, float _startY, float _boxWidth, float _boxHeight, float _spacing, int length, const TTF_Font *_font, SDL_Renderer *_renderer)
    : startX(_startX), startY(_startY), boxWidth(_boxWidth), boxHeight(_boxHeight), spacing(_spacing),
      font(_font), renderer(_renderer), wordLength(length)
{
    for (int i = 0; i < wordLength; ++i)
    {
        float currX = startX + (i * (boxWidth + spacing));

        boxes.emplace_back(currX, startY, boxWidth, boxHeight,
                           " ", CharStatus::UNKNOWN, font, renderer);
    }
}

void WordRow::setWord(const std::string &word, CharStatus defaultStatus)
{
    std::vector<std::string> wordChars = splitUtf8String(word);

    for (int i = 0; i < wordLength; ++i)
    {
        float currX = startX + (i * (boxWidth + spacing));

        std::string chr = (i < wordChars.size()) ? wordChars[i] : " ";

        chr = utf8ToUpper(chr);

        boxes[i] = CharBox(currX, startY, boxWidth, boxHeight,
                           chr, defaultStatus, font, renderer);
    }
}

void WordRow::setStatuses(const std::string &word, const std::vector<CharStatus> &statuses)
{
    if (statuses.size() != wordLength)
    {
        return;
    }

    std::vector<std::string> wordChars = splitUtf8String(word);

    if (wordChars.size() > wordLength)
    {
        wordChars.resize(wordLength);
    }

    for (int i = 0; i < wordLength; ++i)
    {
        float currX = startX + (i * (boxWidth + spacing));

        std::string chr = (i < wordChars.size()) ? wordChars[i] : " ";

        chr = utf8ToUpper(chr);

        boxes[i] = CharBox(currX, startY, boxWidth, boxHeight,
                           chr, statuses[i], font, renderer);
    }
}

void WordRow::render()
{
    for (auto &box : boxes)
    {
        box.render();
    }
}