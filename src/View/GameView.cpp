#include "GameView.hpp"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "CharBox.hpp"
#include "GameLayout.hpp"
#include "KeyboardDisplay.hpp"
#include "WordRow.hpp"

static int findOptimalFontSize(TTF_Text *textObject, const std::string &fontName, AssetManager &assets,
                               int minPT, int maxPT, int targetWidth, int targetHeight)
{
    int bestSize = minPT;
    int low = minPT;
    int high = maxPT;
    int measuredW = 0;
    int measuredH = 0;

    while (low <= high)
    {
        int mid = low + ((high - low) / 2);
        if (mid == 0)
        {
            break;
        }

        assets.setFontSize(fontName, (float)mid);
        TTF_GetTextSize(textObject, &measuredW, &measuredH);

        if (measuredW <= targetWidth && measuredH <= targetHeight)
        {
            bestSize = mid;
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    return bestSize;
}

static CharStatus toCharStatus(GameStateDTO::LetterStatus status)
{
    switch (status)
    {
    case GameStateDTO::LetterStatus::IS_NOT_IN_WORD:
        return CharStatus::ABSENT;
    case GameStateDTO::LetterStatus::IS_IN_WORD_NOT_IN_PLACE:
        return CharStatus::PRESENT;
    case GameStateDTO::LetterStatus::IS_IN_PLACE:
        return CharStatus::CORRECT;
    case GameStateDTO::LetterStatus::IS_NOT_USED:
    default:
        return CharStatus::UNKNOWN;
    }
}

GameView::GameView(SDL_Renderer *_renderer, AssetManager &_assets) : renderer(_renderer), assets(_assets),
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
    clearTextCaches();
    if (c_textEngine != nullptr)
    {
        TTF_DestroyRendererTextEngine(c_textEngine);
    }
}

void GameView::clearTextCaches()
{
    if (c_overlayTitleText != nullptr)
    {
        TTF_DestroyText(c_overlayTitleText);
        c_overlayTitleText = nullptr;
    }
    if (c_overlayRestartText != nullptr)
    {
        TTF_DestroyText(c_overlayRestartText);
        c_overlayRestartText = nullptr;
    }
    if (c_debugFPSText != nullptr)
    {
        TTF_DestroyText(c_debugFPSText);
        c_debugFPSText = nullptr;
    }
    if (c_debugMSText != nullptr)
    {
        TTF_DestroyText(c_debugMSText);
        c_debugMSText = nullptr;
    }

    // clear text cache and state cache
    c_cachedGuessCount = -1;
    c_cachedCurrentInput.clear();
    c_cachedKeyStatuses.clear();
    c_cachedFPS = -1;
    c_cachedMS = -1;
}

void GameView::rebuildUI(const GameStateDTO &state)
{
    // calculate layout
    layout = std::make_unique<GameLayout>(logicalWidth, logicalHeight, state.targetWordLength, state.maxAttempts);
    metrics = layout->getMetrics();

    // fonst loading
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
        return;
    }

    // clear all cache
    wordRows.clear();
    wordRows.reserve(state.maxAttempts);
    keyboard.reset();
    clearTextCaches();

    // create rows
    for (int i = 0; i < state.maxAttempts; ++i)
    {
        float yPos = metrics.startY + (i * (metrics.rowHeight + metrics.rowSpacing));
        wordRows.emplace_back(metrics.startX, yPos,
                              metrics.boxSize, metrics.rowHeight,
                              metrics.boxSpacing, state.targetWordLength,
                              gridFont, renderer, c_textEngine);
    }

    // create keyboard
    keyboard = std::make_unique<KeyboardDisplay>(metrics.keyboardCenterX, metrics.keyboardStartY,
                                                 metrics.keyWidth, metrics.keyHeight,
                                                 metrics.rowSpacing, metrics.keySpacing,
                                                 keyboardFont, renderer, state.currentAlphabet,
                                                 c_textEngine);

    // =============
    // STATE FILLING
    // =============
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

    // keyboard
    for (const auto &[key, status] : state.currentAlphabetStatus)
    {
        keyboard->updateStatus(key, toCharStatus(status));
        c_cachedKeyStatuses[key] = status;
    }

    // clear overlay cache
    c_cachedTitleFontSizePT = 0;
    c_cachedRestartFontSizePT = 0;
}

void GameView::render(const GameStateDTO &state)
{
    SDL_RendererLogicalPresentation mode{SDL_LOGICAL_PRESENTATION_LETTERBOX};
    SDL_GetRenderLogicalPresentation(renderer, &logicalWidth, &logicalHeight, &mode);

    if (logicalWidth != c_cachedLogicalWidth || logicalHeight != c_cachedLogicalHeight || !layout ||
        lastIsGameFinishedState != state.isGameFinished)
    {
        c_cachedLogicalWidth = logicalWidth;
        c_cachedLogicalHeight = logicalHeight;
        lastIsGameFinishedState = state.isGameFinished;
        rebuildUI(state);
    }

    if (wordRows.empty() || !keyboard)
    {
        SDL_Log("UI not initialized, skipping render.");
        return;
    }

    SDL_SetRenderDrawColor(renderer, 18, 18, 19, 255);
    SDL_RenderClear(renderer);

    // REDNDER ONLY DELTA WITH PREVIOUS FRAME

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
        statuses.resize(state.targetWordLength, CharStatus::ABSENT);

        wordRows[rowIndexToUpdate].setStatuses(state.userGuesses[rowIndexToUpdate], statuses);

        c_cachedGuessCount = newGuessCount;
    }

    // current input
    const std::string &newCurrentInput = state.getCurrentInputString();
    if (newCurrentInput != c_cachedCurrentInput && c_cachedGuessCount < state.maxAttempts)
    {
        wordRows[c_cachedGuessCount].setWord(newCurrentInput, CharStatus::UNKNOWN);
        c_cachedCurrentInput = newCurrentInput;
    }

    // ALWAYS RENDER ALL ROWS
    for (auto &row : wordRows)
    {
        row.render();
    }

    // UPDATE KEYBOARD WITH DELTA
    for (const auto &[key, newStatus] : state.currentAlphabetStatus)
    {
        if (!c_cachedKeyStatuses.contains(key) || c_cachedKeyStatuses[key] != newStatus)
        {
            keyboard->updateStatus(key, toCharStatus(newStatus));
            c_cachedKeyStatuses[key] = newStatus;
        }
    }

    // ALWAYS RENDER KEYBOARD
    keyboard->render();

    // OVERLAY RENDER
    if (state.isGameFinished)
    {
        renderFinishOverlay(state, metrics);
    }
}

void GameView::renderText(const std::string &text, int x, int y, TTF_Font *font, const SDL_Color &color, bool center)
{
    if ((font == nullptr) || text.empty() || c_textEngine == nullptr)
    {
        return;
    }

    TTF_Text *textObj = TTF_CreateText(c_textEngine, font, text.c_str(), 0);
    if (textObj == nullptr)
    {
        SDL_Log("TTF_CreateText failed in renderText: %s", SDL_GetError());
        return;
    }

    TTF_SetTextColor(textObj, color.r, color.g, color.b, color.a);

    int textWidth = 0;
    int textHeight = 0;
    TTF_GetTextSize(textObj, &textWidth, &textHeight);

    auto textX = static_cast<float>(x);
    auto textY = static_cast<float>(y);

    if (center)
    {
        textX = x - (textWidth / 2.0F);
        textY = y - (textHeight / 2.0F);
    }

    TTF_DrawRendererText(textObj, textX, textY);
    TTF_DestroyText(textObj);
}

void GameView::renderDebugInfo(int fps, int msPerFrame)
{
    const std::string DEBUG_FONT_NAME = "debug_font";
    const int DEBUG_FONT_PTSIZE = 20;
    const std::string FONT_PATH = DATA_DIR "/fonts/arial.ttf";
    const SDL_Color TEXT_COLOR = {255, 255, 0, 255};

    assets.loadFonts(DEBUG_FONT_NAME, FONT_PATH, DEBUG_FONT_PTSIZE);
    auto *debugFont = const_cast<TTF_Font *>(assets.getFont(DEBUG_FONT_NAME));
    if (debugFont == nullptr || c_textEngine == nullptr)
    {
        return;
    }

    if (c_debugFPSText == nullptr)
    {
        c_debugFPSText = TTF_CreateText(c_textEngine, debugFont, "FPS: ...", 0);
        TTF_SetTextColor(c_debugFPSText, TEXT_COLOR.r, TEXT_COLOR.g, TEXT_COLOR.b, TEXT_COLOR.a);
    }
    if (c_debugMSText == nullptr)
    {
        c_debugMSText = TTF_CreateText(c_textEngine, debugFont, "Frame: ...", 0);
        TTF_SetTextColor(c_debugMSText, TEXT_COLOR.r, TEXT_COLOR.g, TEXT_COLOR.b, TEXT_COLOR.a);
    }

    if (fps != c_cachedFPS)
    {
        std::string fpsText = "FPS: " + std::to_string(fps);
        TTF_SetTextString(c_debugFPSText, fpsText.c_str(), 0);
        c_cachedFPS = fps;
    }

    if (msPerFrame != c_cachedMS)
    {
        std::string msText = "Frame: " + std::to_string(msPerFrame) + " ms";
        TTF_SetTextString(c_debugMSText, msText.c_str(), 0);
        c_cachedMS = msPerFrame;
    }

    const int MARGIN_X = 10;
    const int MARGIN_Y = 10;
    const int LINE_SPACING = 5;

    int fpsW = 0;
    int fspH = 0;
    TTF_GetTextSize(c_debugFPSText, &fpsW, &fspH);

    TTF_DrawRendererText(c_debugFPSText, (float)MARGIN_X, (float)MARGIN_Y);
    TTF_DrawRendererText(c_debugMSText, (float)MARGIN_X, (float)MARGIN_Y + fspH + LINE_SPACING);
}

void GameView::renderFinishOverlay(const GameStateDTO &state, const LayoutMetrics &metrics)
{
    if (c_cachedTitleFontSizePT == 0 || c_cachedRestartFontSizePT == 0 || c_cachedTitleMessage != state.finalMessage)
    {
        c_cachedTitleMessage = state.finalMessage;
        calculateAndCacheOverlayFontSizes(state, metrics);
    }

    if (c_overlayTitleText == nullptr || c_overlayRestartText == nullptr)
    {
        return;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, nullptr);

    const float BOX_WIDTH = metrics.overlayBoxWidth;
    const float BOX_HEIGHT = metrics.overlayBoxHeight;
    const float BOX_X = metrics.overlayBoxX;
    const float BOX_Y = metrics.overlayBoxY;

    // render message box
    SDL_FRect messageBoxRect = {BOX_X, BOX_Y, BOX_WIDTH, BOX_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &messageBoxRect);

    SDL_Color textColor = {0, 0, 0, 255};
    TTF_SetTextColor(c_overlayTitleText, textColor.r, textColor.g, textColor.b, textColor.a);
    TTF_SetTextColor(c_overlayRestartText, textColor.r, textColor.g, textColor.b, textColor.a);

    int finalTitleW = 0;
    int finalTitleH = 0;
    TTF_GetTextSize(c_overlayTitleText, &finalTitleW, &finalTitleH);

    // alignment
    const float DESIRED_TITLE_HEIGHT = metrics.overlayTitleAreaHeight;
    float titleX = BOX_X + ((BOX_WIDTH - finalTitleW) / 2.0f);
    float titleY = BOX_Y + ((DESIRED_TITLE_HEIGHT - finalTitleH) / 2.0f);

    TTF_DrawRendererText(c_overlayTitleText, titleX, titleY);

    int finalRestartW = 0;
    int finalRestartH = 0;
    TTF_GetTextSize(c_overlayRestartText, &finalRestartW, &finalRestartH);

    // alignment
    const float RESTART_Y_POSITION = metrics.overlayRestartYPosition;
    float restartX = BOX_X + ((BOX_WIDTH - finalRestartW) / 2.0f);
    float restartY = RESTART_Y_POSITION - (finalRestartH / 2.0f);

    TTF_DrawRendererText(c_overlayRestartText, restartX, restartY);
}

void GameView::calculateAndCacheOverlayFontSizes(const GameStateDTO &state, const LayoutMetrics &metrics)
{
    if (c_overlayTitleText != nullptr)
    {
        TTF_DestroyText(c_overlayTitleText);
    }
    if (c_overlayRestartText != nullptr)
    {
        TTF_DestroyText(c_overlayRestartText);
    }
    c_overlayTitleText = nullptr;
    c_overlayRestartText = nullptr;

    // fonts constants
    const int MAX_REASONABLE_PTSIZE = 256;
    const int MIN_FONT_SIZE = 10;
    const int MIN_RESTART_FONT_PTSIZE = 12;
    const std::string OVERLAY_FONT_NAME = "overlay_font";
    const std::string RESTART_FONT_NAME = "restart_font";
    const std::string RESTART_MSG = "Нажмите ENTER для новой игры";

    if (c_textEngine == nullptr)
    {
        return;
    }

    // loading fonts
    assets.loadFonts(OVERLAY_FONT_NAME, DATA_DIR "/fonts/arial.ttf", MAX_REASONABLE_PTSIZE);
    assets.loadFonts(RESTART_FONT_NAME, DATA_DIR "/fonts/arial.ttf", MAX_REASONABLE_PTSIZE);
    auto *overlayFont = const_cast<TTF_Font *>(assets.getFont(OVERLAY_FONT_NAME));
    auto *restartFont = const_cast<TTF_Font *>(assets.getFont(RESTART_FONT_NAME));
    if (overlayFont == nullptr)
    {
        return;
    }

    TTF_Text *titleTextObject = TTF_CreateText(c_textEngine, overlayFont, c_cachedTitleMessage.c_str(), 0);
    TTF_Text *restartTextObject = TTF_CreateText(c_textEngine, restartFont, RESTART_MSG.c_str(), 0);

    if (titleTextObject == nullptr || restartTextObject == nullptr)
    {
        if (titleTextObject != nullptr)
        {
            TTF_DestroyText(titleTextObject);
        }
        if (restartTextObject != nullptr)
        {
            TTF_DestroyText(restartTextObject);
        }
        return;
    }

    c_cachedTitleFontSizePT = findOptimalFontSize(titleTextObject, OVERLAY_FONT_NAME, assets,
                                                  MIN_FONT_SIZE, MAX_REASONABLE_PTSIZE,
                                                  metrics.overlayTitleAreaWidth,
                                                  metrics.overlayTitleAreaHeight);

    if (c_cachedRestartFontSizePT == 0)
    {
        c_cachedRestartFontSizePT = findOptimalFontSize(restartTextObject, RESTART_FONT_NAME, assets,
                                                        MIN_RESTART_FONT_PTSIZE, MAX_REASONABLE_PTSIZE,
                                                        metrics.overlayRestartAreaWidth,
                                                        metrics.overlayRestartAreaHeight);
    }

    TTF_DestroyText(titleTextObject);
    TTF_DestroyText(restartTextObject);

    // caching
    assets.setFontSize(OVERLAY_FONT_NAME, (float)c_cachedTitleFontSizePT);
    c_overlayTitleText = TTF_CreateText(c_textEngine, overlayFont, c_cachedTitleMessage.c_str(), 0);

    assets.setFontSize(RESTART_FONT_NAME, (float)c_cachedRestartFontSizePT);
    c_overlayRestartText = TTF_CreateText(c_textEngine, restartFont, RESTART_MSG.c_str(), 0);
}