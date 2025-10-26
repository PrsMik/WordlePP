#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"

#include "AssetManager.hpp"

class DebugOverlay
{
private:
    SDL_Renderer *renderer;
    AssetManager &assets;
    TTF_TextEngine *c_textEngine;

    // debug cache
    int c_cachedFPS = -1;
    int c_cachedMS = -1;
    TTF_Text *c_debugFPSText = nullptr;
    TTF_Text *c_debugMSText = nullptr;

    void clearTextCaches();

public:
    DebugOverlay(SDL_Renderer *_renderer, AssetManager &_assets, TTF_TextEngine *_engine);
    ~DebugOverlay();

    void render(int fps, int msPerFrame);
};