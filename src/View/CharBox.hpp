#pragma once

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <SDL3/SDL.h>
#include <string>

enum class CharStatus
{
    UNKNOWN,
    ABSENT,  // gray
    PRESENT, // yellow
    CORRECT  // green
};

class CharBox
{
private:
    float x, y;
    float width, height;
    std::string chr;
    SDL_Color backgroundColor;
    SDL_Color textColor;
    const TTF_Font *font;
    SDL_Renderer *renderer;

    static SDL_Color getBackgroundColor(CharStatus status);

public:
    CharBox() = default;

    CharBox(float boxX, float boxY, float width, float height,
            std::string chr, CharStatus status, const TTF_Font *font, SDL_Renderer *renderer);

    void render() const;

    void setStatus(CharStatus status);

    const std::string &getChar() const { return chr; }
};