#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <string>
#include <vector>

#include "../Model/GameState.hpp"
#include "AssetManager.hpp"
#include "GameLayout.hpp"
#include "WordRow.hpp"

class GameGrid
{
private:
    SDL_Renderer *renderer;
    TTF_TextEngine *textEngine;
    const TTF_Font *_gridFont;
    LayoutMetrics metrics;

    std::vector<WordRow> wordRows;
    int targetWordLength;
    int maxAttempts;

    // grid cache
    int c_cachedGuessCount = -1;
    std::string c_cachedCurrentInput;

public:
    GameGrid(SDL_Renderer *_renderer, AssetManager &_assets, TTF_TextEngine *_engine,
             const LayoutMetrics &_metrics, const TTF_Font *_gridFont, const GameStateDTO &state);
    ~GameGrid() = default;

    void render(const GameStateDTO &state);
};