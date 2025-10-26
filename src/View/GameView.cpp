#include "GameView.hpp"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <iostream>
#include <string>

#include "DebugOverlay.hpp"
#include "FinishOverlay.hpp"
#include "GameGrid.hpp"
#include "GameLayout.hpp"
#include "KeyboardDisplay.hpp"

GameView::GameView(SDL_Renderer *_renderer, AssetManager &_assets)
    : renderer(_renderer), assets(_assets),
      lastIsGameFinishedState(false),
      layout(nullptr),
      c_textEngine(TTF_CreateRendererTextEngine(renderer))
{
    if (c_textEngine == nullptr)
    {
        std::cerr << "Ошибка: Не удалось создать TTF_TextEngine: " << SDL_GetError() << "\n";
    }
}

GameView::~GameView()
{
    if (c_textEngine != nullptr)
    {
        TTF_DestroyRendererTextEngine(c_textEngine);
    }
}

void GameView::clearTextCaches()
{
    c_cachedGridFontSizePT = 0;
    c_cachedKeyboardFontSizePT = 0;
}

void GameView::rebuildUI(const GameStateDTO &state)
{
    // 1. calculate layout
    layout = std::make_unique<GameLayout>(logicalWidth, logicalHeight, state.targetWordLength, state.maxAttempts);
    metrics = layout->getMetrics();

    // 2. fonts loading
    const int CHARBOX_PTSIZE = metrics.fontSizePT;
    const int KEYBOARD_PTSIZE = metrics.keyboardFontSizePT;
    const std::string GRID_FONT_NAME = "grid_font";
    const std::string KEYBOARD_FONT_NAME = "keyboard_font";
    if (c_cachedGridFontSizePT != CHARBOX_PTSIZE)
    {
        assets.loadFonts(GRID_FONT_NAME, DATA_DIR "/fonts/arial.ttf", CHARBOX_PTSIZE);
        c_cachedGridFontSizePT = CHARBOX_PTSIZE;
    }
    if (c_cachedKeyboardFontSizePT != KEYBOARD_PTSIZE)
    {
        assets.loadFonts(KEYBOARD_FONT_NAME, DATA_DIR "/fonts/arial.ttf", KEYBOARD_PTSIZE);
        c_cachedKeyboardFontSizePT = KEYBOARD_PTSIZE;
    }

    const TTF_Font *gridFont = assets.getFont(GRID_FONT_NAME);
    const TTF_Font *keyboardFont = assets.getFont(KEYBOARD_FONT_NAME);
    if (gridFont == nullptr || keyboardFont == nullptr)
    {
        SDL_Log("GameView::rebuildUI: Fonts are not loaded.");
        return;
    }

    // 3. clear all cache and components
    grid.reset();
    keyboard.reset();
    finishOverlay.reset();
    debugOverlay.reset();
    clearTextCaches();

    // 4. create components
    grid = std::make_unique<GameGrid>(renderer, assets, c_textEngine, metrics, gridFont, state);
    keyboard = std::make_unique<KeyboardDisplay>(renderer, c_textEngine, metrics, keyboardFont, state);
    finishOverlay = std::make_unique<FinishOverlay>(renderer, assets, c_textEngine);
    debugOverlay = std::make_unique<DebugOverlay>(renderer, assets, c_textEngine);
}

void GameView::render(const GameStateDTO &state)
{
    SDL_RendererLogicalPresentation mode{SDL_LOGICAL_PRESENTATION_LETTERBOX};
    SDL_GetRenderLogicalPresentation(renderer, &logicalWidth, &logicalHeight, &mode);

    // check for rebuild
    if (logicalWidth != c_cachedLogicalWidth || logicalHeight != c_cachedLogicalHeight || !layout ||
        lastIsGameFinishedState != state.isGameFinished)
    {
        c_cachedLogicalWidth = logicalWidth;
        c_cachedLogicalHeight = logicalHeight;
        lastIsGameFinishedState = state.isGameFinished;
        rebuildUI(state);
    }

    if (!grid || !keyboard || !finishOverlay)
    {
        SDL_Log("UI components not initialized, skipping render.");
        return;
    }

    SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g,
                           BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);
    SDL_RenderClear(renderer);

    grid->render(state);
    keyboard->render(state.currentAlphabetStatus);

    finishOverlay->render(state, metrics);
}

void GameView::renderDebugInfo(int fps, int msPerFrame)
{
    if (debugOverlay)
    {
        debugOverlay->render(fps, msPerFrame);
    }
}