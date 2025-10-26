#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <string>

#include "../Model/GameState.hpp"
#include "AssetManager.hpp"
#include "GameLayout.hpp"

class FinishOverlay
{
private:
    SDL_Renderer *renderer;
    AssetManager &assets;
    TTF_TextEngine *c_textEngine;

    // overlay cache
    int c_cachedTitleFontSizePT = 0;
    int c_cachedRestartFontSizePT = 0;
    TTF_Text *c_overlayTitleText = nullptr;
    TTF_Text *c_overlayRestartText = nullptr;
    std::string c_cachedTitleMessage;

    void calculateAndCacheOverlayFontSizes(const GameStateDTO &state, const LayoutMetrics &metrics);
    void clearTextCaches();

public:
    FinishOverlay(SDL_Renderer *_renderer, AssetManager &_assets, TTF_TextEngine *_engine);
    ~FinishOverlay();

    void render(const GameStateDTO &state, const LayoutMetrics &metrics);
};