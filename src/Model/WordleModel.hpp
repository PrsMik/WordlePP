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

    void checkInputWord() override;

    bool isValidInput() override;

    void modifyCurrentInput(const std::string &input) override;

    bool isGameOver() const override;

    bool isUserWin() const override;

    [[nodiscard]] GameStateDTO &getGameState() override;

    ~WordleModel();

    WordleModel(WordleModel &&other) noexcept;

    WordleModel &operator=(WordleModel &&other) noexcept;

    WordleModel(const WordleModel &other) = delete;

    WordleModel &operator=(const WordleModel &other) = delete;
};