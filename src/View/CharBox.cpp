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
                 std::string _chr, CharStatus _status,
                 const TTF_Font *_font, SDL_Renderer *_renderer, TTF_TextEngine *_engine)
    : x(_x), y(_y),
      width(_width), height(_height),
      status(_status),
      font(_font),
      renderer(_renderer),
      engine(_engine),
      textColor({.r = 255, .g = 255, .b = 255, .a = 255}),
      textObject(nullptr)
{
    setChar(_chr);
    setStatus(status);
}

CharBox::~CharBox()
{
    if (textObject != nullptr)
    {
        TTF_DestroyText(textObject);
        textObject = nullptr;
    }
}

void CharBox::setChar(const std::string &newChar)
{
    if (chr == newChar && textObject != nullptr)
    {
        return;
    }

    chr = newChar;

    if (textObject != nullptr)
    {
        TTF_DestroyText(textObject);
        textObject = nullptr;
    }

    if ((engine != nullptr) && (font != nullptr) && !chr.empty() && chr != " ")
    {
        textObject = TTF_CreateText(engine, const_cast<TTF_Font *>(font), chr.c_str(), 0);
        if (textObject == nullptr)
        {
            SDL_Log("TTF_CreateText failed in CharBox::setChar: %s", SDL_GetError());
        }
    }
}

void CharBox::setStatus(CharStatus _status)
{
    backgroundColor = getBackgroundColor(_status);
    status = _status;
}

void CharBox::render() const
{
    SDL_FRect fRect = {this->x, this->y, this->width, this->height};

    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderFillRect(renderer, &fRect);

    // if no cached text to render
    if (textObject == nullptr)
    {
        return;
    }

    TTF_SetTextColor(textObject, textColor.r, textColor.g, textColor.b, textColor.a);

    // get text size for alignment
    int textWidth = 0;
    int textHeight = 0;
    TTF_GetTextSize(textObject, &textWidth, &textHeight);

    float textX = x + ((width - textWidth) / 2.0f);
    float textY = y + ((height - textHeight) / 2.0f);

    TTF_DrawRendererText(textObject, textX, textY);
}

CharBox::CharBox(CharBox &&other) noexcept
    : x(other.x), y(other.y), width(other.width), height(other.height),
      chr(std::move(other.chr)),
      status(other.status),
      font(other.font),
      renderer(other.renderer),
      engine(other.engine),
      backgroundColor(other.backgroundColor),
      textColor(other.textColor),
      textObject(other.textObject)
{
    other.textObject = nullptr; // set to avoid other's destructor
}

CharBox &CharBox::operator=(CharBox &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (textObject != nullptr)
    {
        TTF_DestroyText(textObject);
    }

    x = other.x;
    y = other.y;
    width = other.width;
    height = other.height;
    chr = std::move(other.chr);
    backgroundColor = other.backgroundColor;
    textColor = other.textColor;
    font = other.font;
    renderer = other.renderer;
    engine = other.engine;
    status = other.status;

    textObject = other.textObject;
    other.textObject = nullptr;

    return *this;
}