#include "src/Controller/SDLEngine.hpp"
#include "src/Model//Dictionary/Dictionary.hpp"
#include "src/Model/Dictionary/Alphabet.hpp"
#include "src/Model/WordleModel.hpp"
#include <memory>

int main()
{

    auto lang = Alphabet::Language::ENGLISH;
    auto wrdLen = 5;
    auto maxAttempts = 6;
    auto dict = std::make_unique<Dictionary>(lang, wrdLen);
    auto model = std::make_unique<WordleModel>(lang, std::move(dict), maxAttempts);
    SDLEngine controller(std::move(model));
    controller.init("Hello", 800, 800);
    controller.runGameLoop();
    return 0;
}