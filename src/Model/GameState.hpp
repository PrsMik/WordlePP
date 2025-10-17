#pragma once

#include <concepts>
#include <map>
#include <string>
#include <utility>
#include <vector>

struct GameStateDTO
{
    enum class LetterStatus
    {
        IS_NOT_USED,
        IS_NOT_IN_WORD,
        IS_IN_WORD_NOT_IN_PLACE,
        IS_IN_PLACE
    };

    int targetWordLength;
    int maxAttempts;
    std::string currentAlphabet;
    std::string targetWord;
    std::vector<std::string> userGuesses;
    std::map<char, LetterStatus> lastGuessStatus;
    std::map<char, LetterStatus> currentAlphabetStatus;
    std::string errorMessage;

    GameStateDTO(int _maxAttempts, std::string _currentAlphabet, std::string _wordToGuess) : maxAttempts(_maxAttempts),
                                                                                             currentAlphabet(std::move(_currentAlphabet)),
                                                                                             targetWord(std::move(_wordToGuess))
    {
        targetWordLength = static_cast<int>(targetWord.size());
        for (char chr : currentAlphabet)
        {
            currentAlphabetStatus[chr] = LetterStatus::IS_NOT_USED;
        }
    }

    GameStateDTO(GameStateDTO &&other) noexcept : targetWordLength(other.targetWordLength),
                                                  maxAttempts(other.maxAttempts),
                                                  currentAlphabet(std::move(other.currentAlphabet)),
                                                  targetWord(std::move(other.targetWord)),
                                                  userGuesses(std::move(other.userGuesses)),
                                                  currentAlphabetStatus(std::move(other.currentAlphabetStatus)),
                                                  errorMessage(std::move(other.errorMessage))
    {
    }

    GameStateDTO &operator=(GameStateDTO &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(targetWordLength, other.targetWordLength);
            std::swap(maxAttempts, other.maxAttempts);
            std::swap(currentAlphabet, other.currentAlphabet);
            std::swap(targetWord, other.targetWord);
            std::swap(userGuesses, other.userGuesses);
            std::swap(currentAlphabetStatus, other.currentAlphabetStatus);
            std::swap(errorMessage, other.errorMessage);
        }
        return *this;
    }

    GameStateDTO(const GameStateDTO &other) = delete;

    GameStateDTO &operator=(const GameStateDTO &other) = delete;
};