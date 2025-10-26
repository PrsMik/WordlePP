#pragma once

#include "../Model/GameState.hpp"
#include "CharBox.hpp"

static CharStatus toCharStatus(GameStateDTO::LetterStatus status)
{
    switch (status)
    {
    case GameStateDTO::LetterStatus::IS_NOT_IN_WORD:
        return CharStatus::ABSENT;
    case GameStateDTO::LetterStatus::IS_IN_WORD_NOT_IN_PLACE:
        return CharStatus::PRESENT;
    case GameStateDTO::LetterStatus::IS_IN_PLACE:
        return CharStatus::CORRECT;
    case GameStateDTO::LetterStatus::IS_NOT_USED:
    default:
        return CharStatus::UNKNOWN;
    }
}