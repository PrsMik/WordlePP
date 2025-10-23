#pragma once

#include "AssetManager.hpp"
#include "SDL3/SDL_render.h"

#include "../Model/GameState.hpp"
#include "GameLayout.hpp"

class GameView
{
private:
    SDL_Renderer *renderer;
    AssetManager &assets;

    TTF_TextEngine *_textEngine = nullptr;

    int logicalWidth = 0;
    int logicalHeight = 0;

    // overlay cache
    int _cachedLogicalWidth = 0;
    int _cachedLogicalHeight = 0;
    int _cachedTitleFontSizePT = 0;
    int _cachedRestartFontSizePT = 0;
    int _cachedTitleMessageSize = 0;

    // main fonts cache
    int _cachedGridFontSizePT = 0;
    int _cachedKeyboardFontSizePT = 0;

    void renderText(const std::string &text, int textX, int textY, TTF_Font *font, const SDL_Color &color);
    void renderFinishOverlay(const GameStateDTO &state, int logicalWidth, int logicalHeight, const LayoutMetrics &metrics);

    void calculateAndCacheOverlayFontSizes(const GameStateDTO &state, const LayoutMetrics &metrics);

public:
    GameView(SDL_Renderer *_renderer, AssetManager &_assets);

    void render(const GameStateDTO &state);

    void renderDebugInfo(int fps, int msPerFrame);
};