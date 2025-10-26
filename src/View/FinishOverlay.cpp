#include "FinishOverlay.hpp"
#include "SDL3/SDL_rect.h"
#include <string>

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

FinishOverlay::FinishOverlay(SDL_Renderer *_renderer, AssetManager &_assets, TTF_TextEngine *_engine)
    : renderer(_renderer), assets(_assets), c_textEngine(_engine)
{
}

FinishOverlay::~FinishOverlay()
{
    clearTextCaches();
}

void FinishOverlay::clearTextCaches()
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
}

void FinishOverlay::render(const GameStateDTO &state, const LayoutMetrics &metrics)
{
    if (!state.isGameFinished)
    {
        return;
    }

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

void FinishOverlay::calculateAndCacheOverlayFontSizes(const GameStateDTO &state, const LayoutMetrics &metrics)
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