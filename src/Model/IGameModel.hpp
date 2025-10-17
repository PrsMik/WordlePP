#pragma once

#include <string>

#include "GameState.hpp"

class IGameModel
{
public:
    virtual void startNewGame() = 0;

    virtual void checkInputWord(const std::string &) = 0;

    virtual bool isValidInput(const std::string &) = 0;

    virtual bool isGameOver() const = 0;

    virtual bool isUserWin() const = 0;

    [[nodiscard]] virtual const GameStateDTO &getGameState() const = 0;

    virtual ~IGameModel() = default;
};