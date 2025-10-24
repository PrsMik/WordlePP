#pragma once

#include "AssetManager.hpp"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <map>
#include <memory>
#include <vector>

#include "../Model/GameState.hpp"
#include "GameLayout.hpp"
#include "KeyboardDisplay.hpp"
#include "WordRow.hpp"

class GameView
{
private:
    SDL_Renderer *renderer;
    AssetManager &assets;

    // window and game state
    int logicalWidth = 0;
    int logicalHeight = 0;
    bool lastIsGameFinishedState;

    // game layout
    std::unique_ptr<GameLayout> layout;
    LayoutMetrics metrics;
    std::vector<WordRow> wordRows;
    std::unique_ptr<KeyboardDisplay> keyboard;

    // window cache
    int c_cachedLogicalWidth = 0;
    int c_cachedLogicalHeight = 0;

    // state cache
    int c_cachedGuessCount = -1;
    std::string c_cachedCurrentInput;
    std::map<std::string, GameStateDTO::LetterStatus> c_cachedKeyStatuses;

    // text cache
    TTF_TextEngine *c_textEngine = nullptr;
    int c_cachedGridFontSizePT = 0;
    int c_cachedKeyboardFontSizePT = 0;

    // overlay cache
    int c_cachedTitleFontSizePT = 0;
    int c_cachedRestartFontSizePT = 0;
    TTF_Text *c_overlayTitleText = nullptr;
    TTF_Text *c_overlayRestartText = nullptr;
    std::string c_cachedTitleMessage;

    // debug info cache
    int c_cachedFPS = -1;
    int c_cachedMS = -1;
    TTF_Text *c_debugFPSText = nullptr;
    TTF_Text *c_debugMSText = nullptr;

    void renderText(const std::string &text, int x, int y, TTF_Font *font, const SDL_Color &color, bool center = false);
    void renderFinishOverlay(const GameStateDTO &state, const LayoutMetrics &metrics);
    void calculateAndCacheOverlayFontSizes(const GameStateDTO &state, const LayoutMetrics &metrics);

    void rebuildUI(const GameStateDTO &state);

    void clearTextCaches();

public:
    GameView(SDL_Renderer *_renderer, AssetManager &_assets);
    ~GameView();

    void render(const GameStateDTO &state);
    void renderDebugInfo(int fps, int msPerFrame);
};