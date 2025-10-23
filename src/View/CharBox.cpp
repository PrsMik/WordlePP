#include "CharBox.hpp"
#include "SDL3_ttf/SDL_ttf.h"

SDL_Color CharBox::getBackgroundColor(CharStatus status)
{
    switch (status)
    {
    case CharStatus::ABSENT:
        return {58, 58, 60, 255}; // gray
    case CharStatus::PRESENT:
        return {181, 159, 59, 255}; // yellow
    case CharStatus::CORRECT:
        return {83, 141, 78, 255}; // green
    case CharStatus::UNKNOWN:
    default:
        return {120, 124, 126, 255}; // light-gray
    }
}

CharBox::CharBox(float _x, float _y,
                 float _width, float _height,
                 std::string _chr, CharStatus status,
                 const TTF_Font *_font, SDL_Renderer *_renderer) : x(_x), y(_y), width(_width), height(_height),
                                                                   chr(std::move(_chr)),
                                                                   backgroundColor(getBackgroundColor(status)),
                                                                   textColor({.r = 255, .g = 255, .b = 255, .a = 255}),
                                                                   font(_font),
                                                                   renderer(_renderer)
{
}

void CharBox::setStatus(CharStatus status)
{
    backgroundColor = getBackgroundColor(status);
}

void CharBox::render() const
{
    SDL_FRect fRect = {this->x, this->y, this->width, this->height};

    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

    SDL_RenderFillRect(renderer, &fRect);

    if ((font == nullptr) || chr.empty() || chr == " ")
    {
        return;
    }

    TTF_TextEngine *engine = TTF_CreateRendererTextEngine(renderer);
    if (engine == nullptr)
    {
        SDL_Log("TTF_CreateRendererTextEngine failed: %s", SDL_GetError());
        return;
    }

    TTF_Text *text = TTF_CreateText(engine, const_cast<TTF_Font *>(font), chr.c_str(), 0);
    if (text == nullptr)
    {
        SDL_Log("TTF_CreateText failed in CharBox::render: %s", SDL_GetError());
        TTF_DestroyRendererTextEngine(engine);
        return;
    }

    TTF_SetTextColor(text, textColor.r, textColor.g, textColor.b, textColor.a);

    // get text size for alignment
    int textWidth = 0;
    int textHeight = 0;
    TTF_GetTextSize(text, &textWidth, &textHeight);

    float textX = x + ((width - textWidth) / 2.0f);
    float textY = y + ((height - textHeight) / 2.0f);

    TTF_DrawRendererText(text, textX, textY);

    TTF_DestroyText(text);
    TTF_DestroyRendererTextEngine(engine);
}