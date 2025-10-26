#include "GameGrid.hpp"
#include "ViewUtils.hpp"
#include <string>

GameGrid::GameGrid(SDL_Renderer *_renderer, AssetManager &_assets, TTF_TextEngine *_engine,
                   const LayoutMetrics &_metrics, const TTF_Font *_gridFont, const GameStateDTO &state)
    : renderer(_renderer), textEngine(_engine), metrics(_metrics),
      targetWordLength(state.targetWordLength), maxAttempts(state.maxAttempts)
{
    // STATE FILLING
    wordRows.reserve(state.maxAttempts);
    for (int i = 0; i < state.maxAttempts; ++i)
    {
        float yPos = metrics.startY + (i * (metrics.rowHeight + metrics.rowSpacing));
        wordRows.emplace_back(metrics.startX, yPos,
                              metrics.boxSize, metrics.rowHeight,
                              metrics.boxSpacing, state.targetWordLength,
                              _gridFont, renderer, textEngine);
    }

    int currentRowIndex = 0;

    // guesses history
    const int HISTORY_COUNT = state.userGuesses.size();
    for (int i = 0; i < HISTORY_COUNT; ++i)
    {
        std::vector<CharStatus> statuses;
        const auto &guess = state.userGuessesStatuses[i];
        statuses.reserve(guess.size());
        for (const auto &chr : guess)
        {
            statuses.push_back(toCharStatus(chr.second));
        }
        statuses.resize(state.targetWordLength, CharStatus::ABSENT);
        wordRows[i].setStatuses(state.userGuesses[i], statuses);
        currentRowIndex = i + 1;
    }
    c_cachedGuessCount = currentRowIndex;

    // current input
    if (currentRowIndex < state.maxAttempts)
    {
        c_cachedCurrentInput = state.getCurrentInputString();
        wordRows[currentRowIndex].setWord(c_cachedCurrentInput, CharStatus::UNKNOWN);
        currentRowIndex++;
    }

    // empty rows
    for (int i = currentRowIndex; i < state.maxAttempts; ++i)
    {
        wordRows[i].setWord(" ", CharStatus::UNKNOWN);
    }
}

void GameGrid::render(const GameStateDTO &state)
{
    if (wordRows.empty())
    {
        return;
    }

    // guess history
    int newGuessCount = state.userGuesses.size();
    if (newGuessCount > c_cachedGuessCount)
    {
        int rowIndexToUpdate = c_cachedGuessCount;
        const auto &guessStatus = state.userGuessesStatuses[rowIndexToUpdate];
        std::vector<CharStatus> statuses;
        statuses.reserve(guessStatus.size());
        for (const auto &chr : guessStatus)
        {
            statuses.push_back(toCharStatus(chr.second));
        }
        statuses.resize(targetWordLength, CharStatus::ABSENT);

        wordRows[rowIndexToUpdate].setStatuses(state.userGuesses[rowIndexToUpdate], statuses);

        c_cachedGuessCount = newGuessCount;
    }

    // current input
    const std::string &newCurrentInput = state.getCurrentInputString();
    if (newCurrentInput != c_cachedCurrentInput && c_cachedGuessCount < maxAttempts)
    {
        wordRows[c_cachedGuessCount].setWord(newCurrentInput, CharStatus::UNKNOWN);
        c_cachedCurrentInput = newCurrentInput;
    }

    // ALWAYS RENDER ALL ROWS
    for (auto &row : wordRows)
    {
        row.render();
    }
}