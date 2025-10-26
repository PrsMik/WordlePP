#include "KeyboardDisplay.hpp"
#include "UTF8Utils.hpp"
#include "ViewUtils.hpp"
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

KeyboardDisplay::KeyboardDisplay(SDL_Renderer *_renderer, TTF_TextEngine *_engine, const LayoutMetrics &metrics,
                                 const TTF_Font *_font, const GameStateDTO &state)
    : startX(metrics.keyboardCenterX), startY(metrics.keyboardStartY),
      keyWidth(metrics.keyWidth), keyHeight(metrics.keyHeight),
      rowSpacing(metrics.rowSpacing), keySpacing(metrics.keySpacing),
      font(_font), renderer(_renderer),
      engine(_engine), width(0.0F)
{
    // STATE FILLING
    keyRows = createKeyboardRows(state.currentAlphabet);

    float currentY = startY;
    std::vector<std::string> rowByletters;

    for (const auto &row : keyRows)
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
                                          key, CharStatus::UNKNOWN, font, renderer, engine));

            keyStatuses[key] = CharStatus::UNKNOWN;

            currentX += keyWidth + keySpacing;
        }
        currentY += keyHeight + rowSpacing;
    }
    height = currentY - startY + rowSpacing;

    for (const auto &[key, status] : state.currentAlphabetStatus)
    {
        updateStatus(key, toCharStatus(status));
    }
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

void KeyboardDisplay::render(const std::map<std::string, GameStateDTO::LetterStatus> &newKeyStatuses)
{
    // delta update
    for (const auto &[key, newDTOStatus] : newKeyStatuses)
    {
        if (!c_cachedKeyStatuses.contains(key) || c_cachedKeyStatuses[key] != newDTOStatus)
        {
            updateStatus(key, toCharStatus(newDTOStatus));
            c_cachedKeyStatuses[key] = newDTOStatus;
        }
    }

    for (auto const &[key, box] : keyBoxes)
    {
        box.render();
    }
}