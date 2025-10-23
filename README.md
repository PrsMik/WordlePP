# Wordle Game

This is a C++ implementation of the popular Wordle game, utilizing the SDL3 library for graphics and input.

## Project Description

The project represents a classic Wordle game where the player must guess a hidden word within a limited number of attempts. The architecture is built using **Model-View-Controller (MVC)** principles:

* **Model**: Manages game logic (`WordleModel`), game state (`GameState`), and dictionary access (`Dictionary`, `IDictionary`, `Alphabet`).
* **View**: Responsible for rendering the game board and interface (`GameView`, `AssetManager`, `CharBox`, `GameLayout`, `KeyboardDisplay`, `WordRow`).
* **Controller**: Handles SDL initialization, the main game loop (`SDLEngine`), and user input (`InputHandler`).

## Game Initialization

The primary game parameters are set in the `main.cpp` file:

```cpp
// main.cpp
    // Game language
    const auto LANG = Alphabet::Language::RUSSIAN;

    const int WORD_LEN = 5; // Word length
    const int MAX_ATTEMPTS = 6;// Number of attempts
    const int WIDTH = 800; // Window width
    const int HEIGHT = 800; // Window height

    auto dict = std::make_unique<Dictionary>(LANG, WORD_LEN);
    auto model = std::make_unique<WordleModel>(LANG, std::move(dict), MAX_ATTEMPTS);
    SDLEngine controller(std::move(model));

    controller.init("Wordle Game", WIDTH, HEIGHT);
    controller.runGameLoop();
```
By default, the game is configured for Russian language  and 5-letter words.

## Building and Running
(It is assumed that you have the necessary SDL3 and SDL_ttf dependencies installed.)

1. Clone the repository and submodules.

2. Ensure SDL3 library paths are configured.

3. Compile the project. (It uses CMake and CMakeLists to configure build)

4. Run the executable.