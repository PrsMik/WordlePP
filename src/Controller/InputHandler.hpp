#pragma once

#include "SDL3/SDL_events.h"

#include "../Model/IGameModel.hpp"

class InputHandler
{
private:
    bool errorFlag;
    bool quitFlag;
    std::string inputedText;
    IGameModel *gameModel;

    static bool SDLCALL handle(void *userdata, SDL_Event *event);

public:
    void init(SDL_Window *window, IGameModel *model);

    bool shouldQuit() const;

    void resetState();
};