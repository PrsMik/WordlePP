#include "InputHandler.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_keycode.h"
#include <vector>

#include "SDL3/SDL_video.h"

bool SDLCALL InputHandler::handle(void *userdata, SDL_Event *event)
{
    auto *instance = static_cast<InputHandler *>(userdata);
    instance->errorFlag = false;
    if (event->type == SDL_EVENT_QUIT)
    {
        instance->quitFlag = true;
    }
    else if (event->type == SDL_EVENT_TEXT_INPUT)
    {
        std::string str = event->text.text;
        if (instance->inputedText.size() < instance->gameModel->getGameState().targetWord.size())
        {
            instance->inputedText.append(event->text.text);
            instance->gameModel->modifyCurrentInput(instance->inputedText);
        }
        return true;
    }
    else if (event->type == SDL_EVENT_KEY_DOWN)
    {
        if (event->key.key == SDLK_RETURN)
        {
            if (instance->gameModel->isGameOver())
            {
                SDL_StopTextInput(instance->gameWindow);
                return !instance->handleGameOverInput(event);
            }

            // handle text input
            if (instance->gameModel->getGameState().currentInputByLetters.size() !=
                instance->gameModel->getGameState().targetWordByLetters.size())
            {
                return false;
            }

            // check if word correct and modify gameState after
            if (instance->gameModel->isValidInput())
            {
                instance->gameModel->checkInputWord();
            }

            instance->inputedText.clear();
            instance->gameModel->modifyCurrentInput(instance->inputedText);

            if (instance->gameModel->isGameOver())
            {
                SDL_StopTextInput(instance->gameWindow);
            }

            return false;
        }
        if (event->key.key == SDLK_BACKSPACE)
        {
            std::string &text = instance->inputedText;

            if (!text.empty())
            {
                const char *endPtr = text.c_str() + text.length();

                const char *backPtr = endPtr;

                SDL_StepBackUTF8(text.c_str(), &backPtr);

                if (backPtr != endPtr)
                {
                    size_t startIndex = backPtr - text.c_str();

                    text.erase(startIndex);
                }
            }
            instance->gameModel->modifyCurrentInput(instance->inputedText);
            return true;
        }
        return false;
    }
    return true;
}

void InputHandler::init(SDL_Window *window, IGameModel *model)
{
    gameWindow = window;
    gameModel = model;
    SDL_StartTextInput(window);
    SDL_SetEventFilter(InputHandler::handle, this);
    isInited = true;
}

bool InputHandler::shouldQuit() const
{
    return quitFlag;
}

void InputHandler::resetState(SDL_Window *window)
{
    gameModel = nullptr;
    quitFlag = false;
    SDL_StopTextInput(window);
    SDL_SetEventFilter(nullptr, nullptr);
    isInited = false;
}

bool InputHandler::handleGameOverInput(SDL_Event *event)
{
    if (!this->gameModel->getGameState().isGameFinished)
    {
        return false;
    }

    if (event->type == SDL_EVENT_KEY_DOWN)
    {
        if (event->key.key == SDLK_RETURN)
        {
            this->gameModel->startNewGame();
            SDL_StartTextInput(this->gameWindow);
            return true;
        }
    }
    return false;
}