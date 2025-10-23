#pragma once

#include <string>

#include "GameState.hpp"

class IGameModel
{
public:
    virtual void startNewGame() = 0;

    virtual void checkInputWord() = 0;

    virtual bool isValidInput() = 0;

    virtual void modifyCurrentInput(const std::string &) = 0;

    virtual bool isGameOver() const = 0;

    virtual bool isUserWin() const = 0;

    [[nodiscard]] virtual GameStateDTO &getGameState() = 0;

    virtual ~IGameModel() = default;
};