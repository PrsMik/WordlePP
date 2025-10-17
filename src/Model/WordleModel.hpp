#pragma once

#include <memory>
#include <string>

#include "Dictionary/IDictionary.hpp"
#include "GameState.hpp"
#include "IGameModel.hpp"

class WordleModel : public IGameModel
{
private:
    Alphabet::Language gameLanguage;
    std::unique_ptr<IDictionary> gameDictionary;
    GameStateDTO gameState;

public:
    WordleModel(Alphabet::Language alphabetLanguage, std::unique_ptr<IDictionary> gameDictionary, int maxAttempts);

    void startNewGame() override;

    void checkInputWord(const std::string &inputWord) override;

    bool isValidInput(const std::string &inputWord) override;

    bool isGameOver() const override;

    bool isUserWin() const override;

    [[nodiscard]] const GameStateDTO &getGameState() const override;

    ~WordleModel();

    WordleModel(WordleModel &&other) noexcept;

    WordleModel &operator=(WordleModel &&other) noexcept;

    WordleModel(const WordleModel &other) = delete;

    WordleModel &operator=(const WordleModel &other) = delete;
};