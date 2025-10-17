#pragma once

#include "../Model/IGameModel.hpp"
#include "../View/AssetManager.hpp"
#include "../View/GameView.hpp"
#include "InputHandler.hpp"

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <memory>

class SDLEngine
{
private:
    bool isValid;
    std::unique_ptr<InputHandler> inputHandler;
    std::unique_ptr<IGameModel> game;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> mainWindow;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
    std::unique_ptr<AssetManager> manager;
    std::unique_ptr<GameView> view;

public:
    SDLEngine(std::unique_ptr<IGameModel> gameModel);

    ~SDLEngine();

    void init(const std::string &title, int width, int height);

    void runGameLoop();

    SDLEngine(SDLEngine &&other) = delete;

    SDLEngine &operator=(SDLEngine &&other) = delete;

    SDLEngine(const SDLEngine &other) = delete;

    SDLEngine &operator=(const SDLEngine &other) = delete;
};