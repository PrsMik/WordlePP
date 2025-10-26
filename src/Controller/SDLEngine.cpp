#include "SDLEngine.hpp"
#include "SDL3/SDL_events.h"
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
    TTF_Init();
    SDL_Window *windowPtr = nullptr;
    SDL_Renderer *rendererPtr = nullptr;

    bool success = SDL_CreateWindowAndRenderer(title.c_str(), width, height, SDL_WINDOW_RESIZABLE, &windowPtr, &rendererPtr);

    if (success)
    {
        inputHandler = std::make_unique<InputHandler>();
        mainWindow.reset(windowPtr);
        renderer.reset(rendererPtr);
        SDL_SetRenderVSync(renderer.get(), SDL_RENDERER_VSYNC_DISABLED);
        SDL_SetRenderLogicalPresentation(renderer.get(), 1080, 1920,
                                         SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_LETTERBOX);
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

    Uint64 lastTime = SDL_GetPerformanceCounter();
    int fps = 0;
    int msPerFrame = 0;

    while (!inputHandler->shouldQuit())
    {
        Uint64 now = SDL_GetPerformanceCounter();
        double deltaTime = (double)(now - lastTime) / SDL_GetPerformanceFrequency();
        lastTime = now;

        fps = (int)(1.0 / deltaTime);
        msPerFrame = (int)(deltaTime * 1000.0);

        while (SDL_PollEvent(&event))
        {
        }

        view->render(game->getGameState());

        view->renderDebugInfo(fps, msPerFrame);

        SDL_RenderPresent(renderer.get());
    }
    TTF_Quit();
}