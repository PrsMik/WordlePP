#include "SDLEngine.hpp"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <memory>
#include <stdexcept>

SDLEngine::SDLEngine(std::unique_ptr<IGameModel> gameModel) : isValid(false),
                                                              inputHandler(nullptr),
                                                              game(std::move(gameModel)),
                                                              mainWindow(nullptr, SDL_DestroyWindow),
                                                              renderer(nullptr, SDL_DestroyRenderer),
                                                              manager(nullptr),
                                                              view(nullptr)
{
}

SDLEngine::~SDLEngine()
{
}

void SDLEngine::init(const std::string &title, int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window *windowPtr = nullptr;
    SDL_Renderer *rendererPtr = nullptr;

    bool success = SDL_CreateWindowAndRenderer(title.c_str(), width, height, SDL_WINDOW_RESIZABLE, &windowPtr, &rendererPtr);

    if (success)
    {
        inputHandler = std::make_unique<InputHandler>();
        mainWindow.reset(windowPtr);
        renderer.reset(rendererPtr);
        manager = std::make_unique<AssetManager>(renderer.get());
        view = std::make_unique<GameView>(renderer.get(), *manager);
    }
    else
    {
        throw std::runtime_error("Не удалось создать окно!");
    }
}

void SDLEngine::runGameLoop()
{
    inputHandler->init(mainWindow.get(), game.get());
    SDL_Event event;
    while (!inputHandler->shouldQuit())
    {
        while (SDL_PollEvent(&event))
        {
        }
        SDL_RenderClear(renderer.get());
        SDL_RenderPresent(renderer.get());
    }
}