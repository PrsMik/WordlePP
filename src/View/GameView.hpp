#pragma once

#include "AssetManager.hpp"
#include "SDL3/SDL_render.h"

#include "../Model/GameState.hpp"

class GameView
{
private:
    SDL_Renderer *renderer;
    AssetManager assets;

public:
    GameView(SDL_Renderer *renderer, AssetManager &assets);
    void render(const GameStateDTO &state);
    void renderDebugInfo(int fps, int msPerFrame);
};