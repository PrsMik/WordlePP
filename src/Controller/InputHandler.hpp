#pragma once

#include "SDL3/SDL_events.h"

#include "../Model/IGameModel.hpp"
#include <string>

class InputHandler
{
private:
    bool quitFlag;
    bool errorFlag;
    bool isInited;
    bool gameOver;
    std::string inputedText;
    SDL_Window *gameWindow;
    IGameModel *gameModel;

    static bool SDLCALL handle(void *userdata, SDL_Event *event);

    bool handleGameOverInput(SDL_Event *event);

public:
    void init(SDL_Window *window, IGameModel *model);

    bool shouldQuit() const;

    void resetState(SDL_Window *window);

    bool getIsInited() const { return isInited; };
};