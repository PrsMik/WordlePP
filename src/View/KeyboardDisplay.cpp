#include "KeyboardDisplay.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <vector>

std::vector<std::string> createKeyboardRows(const std::string &alphabet)
{
    std::vector<std::string> allLetters = splitUtf8String(utf8ToUpper(alphabet));

    std::vector<std::string> newRows;
    const int NUM_ROWS = 3;
    if (allLetters.empty())
    {
        return newRows;
    }

    size_t totalLetters = allLetters.size();
    size_t lettersPerRow = (totalLetters + NUM_ROWS - 1) / NUM_ROWS;

    size_t currentStart = 0;
    for (int i = 0; i < NUM_ROWS && currentStart < totalLetters; ++i)
    {
        size_t rowLength = std::min(lettersPerRow, totalLetters - currentStart);
        std::string rowString;
        for (size_t j = 0; j < rowLength; ++j)
        {
            rowString += allLetters[currentStart + j];
        }
        newRows.push_back(rowString);
        currentStart += rowLength;
    }
    return newRows;
}

char toUpper(char c)
{
    return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
}

KeyboardDisplay::KeyboardDisplay(float _startX, float _startY, float _keyWidth, float _keyHeight,
                                 float _rowSpacing, float _keySpacing, const TTF_Font *_font, SDL_Renderer *_renderer,
                                 const std::string &alphabet)
    : renderer(_renderer), font(_font), startX(_startX), startY(_startY),
      keyWidth(_keyWidth), keyHeight(_keyHeight), rowSpacing(_rowSpacing), keySpacing(_keySpacing), width(0.0f)
{
    rows = createKeyboardRows(alphabet);

    float currentY = startY;
    std::vector<std::string> rowByletters;

    for (const auto &row : rows)
    {
        rowByletters = splitUtf8String(row);

        if (rowByletters.empty())
        {
            continue;
        }

        float rowTotalWidth = (rowByletters.size() * keyWidth) + ((rowByletters.size() - 1) * keySpacing);
        width = std::max(rowTotalWidth, width);
        float currentX = startX - (rowTotalWidth / 2.0f);

        for (auto key : rowByletters)
        {
            keyBoxes.emplace(key, CharBox(currentX, currentY, keyWidth, keyHeight,
                                          key, CharStatus::UNKNOWN, font, renderer));

            keyStatuses[key] = CharStatus::UNKNOWN;

            currentX += keyWidth + keySpacing;
        }
        currentY += keyHeight + rowSpacing;
    }
    height = currentY - startY + rowSpacing;
}

void KeyboardDisplay::updateStatus(std::string key, CharStatus status)
{
    key = utf8ToUpper(key);
    if (keyStatuses.contains(key))
    {
        CharStatus currentStatus = keyStatuses[key];
        if (status > currentStatus)
        {
            keyStatuses[key] = status;
            keyBoxes.at(key).setStatus(status);
        }
    }
}

void KeyboardDisplay::render()
{
    for (auto const &[key, box] : keyBoxes)
    {
        box.render();
    }
}