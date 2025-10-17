#include "InputHandler.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include <iostream>

bool SDLCALL InputHandler::handle(void *userdata, SDL_Event *event)
{
    auto *instance = static_cast<InputHandler *>(userdata);
    if (event->type == SDL_EVENT_QUIT)
    {
        instance->quitFlag = true;
    }
    else if (event->type == SDL_EVENT_TEXT_INPUT)
    {
        std::cout << event->text.text;
        instance->inputedText.append(event->text.text);
    }
    else if (event->type == SDL_EVENT_KEY_UP)
    {
        if (event->key.key == SDLK_RETURN)
        {
            std::cout << "\n";
            std::cout << "ENTER was pressed and the text was " << instance->inputedText << "\n";
            if (instance->gameModel->isValidInput(instance->inputedText))
            {
                instance->gameModel->checkInputWord(instance->inputedText);
            }
            instance->inputedText.clear();
        }
        else if (event->key.key == SDLK_BACKSPACE)
        {
            std::cout << "\n";
            instance->inputedText.pop_back();
            std::cout << "BACKSPACE was pressed\n";
        }
    }
    return true;
}

void InputHandler::init(SDL_Window *window, IGameModel *model)
{
    gameModel = model;
    SDL_StartTextInput(window);
    SDL_AddEventWatch(InputHandler::handle, this);
}

bool InputHandler::shouldQuit() const
{
    return quitFlag;
}

void InputHandler::resetState()
{
    quitFlag = false;
}