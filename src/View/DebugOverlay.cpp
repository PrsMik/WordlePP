#include "DebugOverlay.hpp"
#include <string>

DebugOverlay::DebugOverlay(SDL_Renderer *_renderer, AssetManager &_assets, TTF_TextEngine *_engine)
    : renderer(_renderer), assets(_assets), c_textEngine(_engine)
{
}

DebugOverlay::~DebugOverlay()
{
    clearTextCaches();
}

void DebugOverlay::clearTextCaches()
{
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
}

void DebugOverlay::render(int fps, int msPerFrame)
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