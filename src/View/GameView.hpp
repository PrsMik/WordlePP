#pragma once

#include "AssetManager.hpp"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <memory>

#include "../Model/GameState.hpp"
#include "DebugOverlay.hpp"
#include "FinishOverlay.hpp"
#include "GameGrid.hpp"
#include "GameLayout.hpp"
#include "KeyboardDisplay.hpp"

class GameView
{
private:
    SDL_Renderer *renderer;
    AssetManager &assets;

    // window state
    int logicalWidth = 0;
    int logicalHeight = 0;
    bool lastIsGameFinishedState;

    // game layout
    const SDL_Color BACKGROUND_COLOR = {255, 100, 100, 255};
    std::unique_ptr<GameLayout> layout;
    LayoutMetrics metrics;

    // ui components
    std::unique_ptr<GameGrid> grid;
    std::unique_ptr<KeyboardDisplay> keyboard;
    std::unique_ptr<FinishOverlay> finishOverlay;
    std::unique_ptr<DebugOverlay> debugOverlay;

    // window cache
    int c_cachedLogicalWidth = 0;
    int c_cachedLogicalHeight = 0;

    // Text Engine
    TTF_TextEngine *c_textEngine = nullptr;

    // font's cache
    int c_cachedGridFontSizePT = 0;
    int c_cachedKeyboardFontSizePT = 0;

    void rebuildUI(const GameStateDTO &state);
    void clearTextCaches();

public:
    GameView(SDL_Renderer *_renderer, AssetManager &_assets);
    ~GameView();

    void render(const GameStateDTO &state);
    void renderDebugInfo(int fps, int msPerFrame);
};