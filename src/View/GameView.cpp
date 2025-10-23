#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "CharBox.hpp"
#include "GameLayout.hpp"
#include "GameView.hpp"
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
                                                                     _textEngine(TTF_CreateRendererTextEngine(renderer))
{
    if (_textEngine == nullptr)
    {
        std::cerr << "Ошибка: Не удалось создать TTF_TextEngine: " << SDL_GetError() << std::endl;
    }
}

void GameView::render(const GameStateDTO &state)
{
    SDL_RendererLogicalPresentation mode{SDL_LOGICAL_PRESENTATION_LETTERBOX};
    SDL_GetRenderLogicalPresentation(renderer, &logicalWidth, &logicalHeight, &mode);

    if (logicalWidth != _cachedLogicalWidth || logicalHeight != _cachedLogicalHeight)
    {
        _cachedLogicalWidth = logicalWidth;
        _cachedLogicalHeight = logicalHeight;

        _cachedTitleFontSizePT = 0;
        _cachedRestartFontSizePT = 0;

        _cachedGridFontSizePT = 0;
        _cachedKeyboardFontSizePT = 0;
    }

    GameLayout layout(logicalWidth, logicalHeight, state.targetWordLength, state.maxAttempts);
    LayoutMetrics metrics = layout.getMetrics();

    const int WORD_LENGTH = state.targetWordLength;
    const int MAX_GUESSES = state.maxAttempts;

    const float BOX_SIZE = metrics.boxSize;
    const float BOX_HEIGHT = metrics.rowHeight;
    const float BOX_SPACING = metrics.boxSpacing;
    const float GRID_ROW_SPACING = metrics.rowSpacing;

    const float GRID_ROW_STEP_Y = BOX_HEIGHT + GRID_ROW_SPACING;

    const float START_X = metrics.startX;
    const float START_Y = metrics.startY;

    const float KEYBOARD_START_Y = metrics.keyboardStartY;
    const float KEY_WIDTH = metrics.keyWidth;
    const float KEY_HEIGHT = metrics.keyHeight;
    const float KEY_SPACING = metrics.keySpacing;
    const float ROW_SPACING = metrics.rowSpacing;
    const float KEYBOARD_CENTER_X = metrics.keyboardCenterX;

    const int CHARBOX_PTSIZE = metrics.fontSizePT;
    const int KEYBOARD_PTSIZE = metrics.keyboardFontSizePT;
    const std::string FONT_NAME = "arial";
    const std::string GRID_FONT_NAME = "grid_font";
    const std::string KEYBOARD_FONT_NAME = "keyboard_font";

    if (_cachedGridFontSizePT != CHARBOX_PTSIZE)
    {
        assets.loadFonts(GRID_FONT_NAME, DATA_DIR "/fonts/arial.ttf", CHARBOX_PTSIZE);
        _cachedGridFontSizePT = CHARBOX_PTSIZE;
    }

    if (_cachedKeyboardFontSizePT != KEYBOARD_PTSIZE)
    {
        assets.loadFonts(KEYBOARD_FONT_NAME, DATA_DIR "/fonts/arial.ttf", KEYBOARD_PTSIZE);
        _cachedKeyboardFontSizePT = KEYBOARD_PTSIZE;
    }

    const TTF_Font *gridFont = assets.getFont(GRID_FONT_NAME);
    const TTF_Font *keyboardFont = assets.getFont(KEYBOARD_FONT_NAME);
    if (gridFont == nullptr || keyboardFont == nullptr)
    {
        SDL_Log("Font not loaded. Cannot render.");
        return;
    }

    SDL_SetRenderDrawColor(renderer, 18, 18, 19, 255);
    SDL_RenderClear(renderer);

    std::vector<WordRow> wordRows;
    int currentRowIndex = 0;

    // render history
    const int HISTORY_COUNT = state.userGuesses.size();
    for (int i = 0; i < HISTORY_COUNT; ++i)
    {
        float yPos = START_Y + (i * GRID_ROW_STEP_Y);

        wordRows.emplace_back(START_X, yPos, BOX_SIZE, BOX_HEIGHT,
                              BOX_SPACING, WORD_LENGTH, gridFont, renderer);

        if (!state.userGuessesStatuses.empty())
        {
            std::vector<CharStatus> statuses;
            const auto &guess = state.userGuessesStatuses[i];
            for (const auto &chr : guess)
            {
                CharStatus status = toCharStatus(chr.second);
                statuses.push_back(status);
            }
            statuses.resize(WORD_LENGTH, CharStatus::ABSENT);

            wordRows.back().setStatuses(state.userGuesses[i], statuses);
        }
        currentRowIndex = i + 1;
    }

    // render current input
    if (currentRowIndex < MAX_GUESSES)
    {
        float yPos = START_Y + (currentRowIndex * GRID_ROW_STEP_Y);

        wordRows.emplace_back(START_X, yPos, BOX_SIZE,
                              BOX_HEIGHT, BOX_SPACING, WORD_LENGTH, gridFont, renderer);

        wordRows.back().setWord(state.getCurrentInputString(), CharStatus::UNKNOWN);

        currentRowIndex++;
    }

    // render unused attempts
    for (int i = currentRowIndex; i < MAX_GUESSES; ++i)
    {
        float yPos = START_Y + (i * GRID_ROW_STEP_Y);

        wordRows.emplace_back(START_X, yPos, BOX_SIZE,
                              BOX_HEIGHT, BOX_SPACING, WORD_LENGTH, gridFont, renderer);
    }

    // render all rows
    for (auto &row : wordRows)
    {
        row.render();
    }

    // render keyboard
    KeyboardDisplay keyboard(KEYBOARD_CENTER_X, KEYBOARD_START_Y,
                             KEY_WIDTH, KEY_HEIGHT, ROW_SPACING, KEY_SPACING,
                             keyboardFont, renderer, state.currentAlphabet);

    // set keys statuses in keyboard
    for (const auto &[key, status] : state.currentAlphabetStatus)
    {
        keyboard.updateStatus(key, toCharStatus(status));
    }

    keyboard.render();

    if (state.isGameFinished)
    {
        renderFinishOverlay(state, logicalWidth, logicalHeight, metrics);
    }
}

void GameView::renderText(const std::string &text, int x, int y, TTF_Font *font, const SDL_Color &color)
{
    if ((font == nullptr) || text.empty())
    {
        return;
    }

    SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), 0, color);
    if (surface == nullptr)
    {
        SDL_Log("TTF_RenderText_Blended Error: %s", SDL_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr)
    {
        SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        return;
    }

    SDL_FRect dstRect;
    dstRect.x = static_cast<float>(x);
    dstRect.y = static_cast<float>(y);
    dstRect.w = static_cast<float>(surface->w);
    dstRect.h = static_cast<float>(surface->h);

    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}

void GameView::renderDebugInfo(int fps, int msPerFrame)
{
    const std::string DEBUG_FONT_NAME = "debug_font";
    const int DEBUG_FONT_PTSIZE = 20;
    const std::string FONT_PATH = DATA_DIR "/fonts/arial.ttf";
    const SDL_Color TEXT_COLOR = {255, 255, 0, 255};

    assets.loadFonts(DEBUG_FONT_NAME, FONT_PATH, DEBUG_FONT_PTSIZE);
    TTF_Font *debugFont = const_cast<TTF_Font *>(assets.getFont(DEBUG_FONT_NAME));

    if (debugFont == nullptr)
    {
        return;
    }

    std::string fpsText = "FPS: " + std::to_string(fps);
    std::string msText = "Frame: " + std::to_string(msPerFrame) + " ms";

    const int MARGIN_X = 10;
    const int MARGIN_Y = 10;
    const int LINE_SPACING = 5;

    int fpsW = 0;
    int fspH = 0;
    TTF_GetStringSize(debugFont, fpsText.c_str(), 0, &fpsW, &fspH);

    renderText(fpsText, MARGIN_X, MARGIN_Y, debugFont, TEXT_COLOR);

    renderText(msText, MARGIN_X, MARGIN_Y + fspH + LINE_SPACING, debugFont, TEXT_COLOR);
}

void GameView::renderFinishOverlay(const GameStateDTO &state, int logicalWidth, int logicalHeight, const LayoutMetrics &metrics)
{
    if (_cachedTitleFontSizePT == 0 || _cachedRestartFontSizePT == 0 || _cachedTitleMessageSize != state.finalMessage.size())
    {
        calculateAndCacheOverlayFontSizes(state, metrics);
    }

    // background blur
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, nullptr);

    const float BOX_WIDTH = metrics.overlayBoxWidth;
    const float BOX_HEIGHT = metrics.overlayBoxHeight;
    const float BOX_X = metrics.overlayBoxX;
    const float BOX_Y = metrics.overlayBoxY;

    // render message rect
    SDL_FRect messageBoxRect = {BOX_X, BOX_Y, BOX_WIDTH, BOX_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &messageBoxRect);

    SDL_Color textColor = {0, 0, 0, 255};

    const int CALCULATED_TITLE_FONT_SIZE_PT = _cachedTitleFontSizePT;
    const std::string OVERLAY_FONT_NAME = "overlay_font";

    assets.setFontSize(OVERLAY_FONT_NAME, (float)CALCULATED_TITLE_FONT_SIZE_PT);
    TTF_Font *titleFont = const_cast<TTF_Font *>(assets.getFont(OVERLAY_FONT_NAME));

    // get text size for alignment
    int finalTitleW = 0;
    int finalTitleH = 0;
    if (titleFont != nullptr)
    {
        TTF_GetStringSize(titleFont, state.finalMessage.c_str(), 0, &finalTitleW, &finalTitleH);
    }

    // render game end message
    const float DESIRED_TITLE_HEIGHT = metrics.overlayTitleAreaHeight;
    renderText(state.finalMessage,
               (int)(BOX_X + ((BOX_WIDTH - finalTitleW) / 2)),
               (int)(BOX_Y + ((DESIRED_TITLE_HEIGHT - finalTitleH) / 2.0f)),
               titleFont, textColor);

    const std::string RESTART_MSG = "Нажмите ENTER для новой игры";
    const int CALCULATED_RESTART_FONT_SIZE_PT = _cachedRestartFontSizePT;

    assets.setFontSize(OVERLAY_FONT_NAME, (float)CALCULATED_RESTART_FONT_SIZE_PT);
    TTF_Font *restartFont = const_cast<TTF_Font *>(assets.getFont(OVERLAY_FONT_NAME));

    // get text size for alignment
    int finalRestartW = 0;
    int finalRestartH = 0;
    if (restartFont != nullptr)
    {
        TTF_GetStringSize(restartFont, RESTART_MSG.c_str(), 0, &finalRestartW, &finalRestartH);
    }

    const float RESTART_Y_POSITION = metrics.overlayRestartYPosition;

    // render restart instruction
    renderText(RESTART_MSG,
               (int)(BOX_X + ((BOX_WIDTH - finalRestartW) / 2)),
               (int)(RESTART_Y_POSITION - (finalRestartH / 2.0f)),
               restartFont, textColor);
}

void GameView::calculateAndCacheOverlayFontSizes(const GameStateDTO &state, const LayoutMetrics &metrics)
{
    const int MAX_REASONABLE_PTSIZE = 256;

    const int MIN_FONT_SIZE = 10;
    const int MIN_RESTART_FONT_PTSIZE = 12;

    const std::string OVERLAY_FONT_NAME = "overlay_font";
    const std::string RESTART_MSG = "Нажмите ENTER для новой игры";

    if (_textEngine == nullptr)
    {
        _cachedTitleFontSizePT = 0;
        _cachedRestartFontSizePT = 0;
        return;
    }

    assets.loadFonts(OVERLAY_FONT_NAME, DATA_DIR "/fonts/arial.ttf", MAX_REASONABLE_PTSIZE);
    TTF_Font *overlayFont = const_cast<TTF_Font *>(assets.getFont(OVERLAY_FONT_NAME));

    if (overlayFont == nullptr)
    {
        _cachedTitleFontSizePT = 0;
        _cachedRestartFontSizePT = 0;
        return;
    }

    std::string titleMessage = state.finalMessage;
    if (_cachedTitleMessageSize == 0)
    {
        titleMessage = state.LOSE_MESSAGE.length() > state.WIN_MESSAGE.length() ? state.LOSE_MESSAGE : state.WIN_MESSAGE;
    }
    _cachedTitleMessageSize = titleMessage.size();

    TTF_Text *titleTextObject = TTF_CreateText(_textEngine, overlayFont, titleMessage.c_str(), 0);
    TTF_Text *restartTextObject = TTF_CreateText(_textEngine, overlayFont, RESTART_MSG.c_str(), 0);

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
        _cachedTitleFontSizePT = 0;
        _cachedRestartFontSizePT = 0;
        return;
    }

    // calculate game end text size with binary search
    _cachedTitleFontSizePT = findOptimalFontSize(titleTextObject, OVERLAY_FONT_NAME, assets,
                                                 MIN_FONT_SIZE, MAX_REASONABLE_PTSIZE,
                                                 metrics.overlayTitleAreaWidth, metrics.overlayTitleAreaHeight);

    // calculate if only not cached
    if (_cachedRestartFontSizePT == 0)
    {
        _cachedRestartFontSizePT = findOptimalFontSize(restartTextObject, OVERLAY_FONT_NAME, assets,
                                                       MIN_RESTART_FONT_PTSIZE, MAX_REASONABLE_PTSIZE,
                                                       metrics.overlayRestartAreaWidth, metrics.overlayRestartAreaHeight);
    }

    TTF_DestroyText(titleTextObject);
    TTF_DestroyText(restartTextObject);
}