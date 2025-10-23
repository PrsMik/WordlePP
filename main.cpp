#include "src/Controller/SDLEngine.hpp"
#include "src/Model//Dictionary/Dictionary.hpp"
#include "src/Model/Dictionary/Alphabet.hpp"
#include "src/Model/WordleModel.hpp"
#include <memory>

int main()
{

    const auto LANG = Alphabet::Language::RUSSIAN;
    const int WORD_LEN = 5;
    const int MAX_ATTEMPTS = 6;
    const int WIDTH = 800;
    const int HEIGHT = 800;

    auto dict = std::make_unique<Dictionary>(LANG, WORD_LEN);
    auto model = std::make_unique<WordleModel>(LANG, std::move(dict), MAX_ATTEMPTS);
    SDLEngine controller(std::move(model));

    controller.init("Wordle Game", WIDTH, HEIGHT);
    controller.runGameLoop();
    return 0;
}