#pragma once

#include <format>
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

    bool isGameFinished;
    int targetWordLength;
    int maxAttempts;

    std::string targetWord;
    std::vector<std::string> currentInputByLetters;
    std::vector<std::string> targetWordByLetters;

    std::string currentAlphabet;
    std::map<std::string, LetterStatus> currentAlphabetStatus;

    std::vector<std::string> userGuesses;
    std::vector<std::pair<std::string, LetterStatus>> lastGuessStatus;
    std::vector<std::vector<std::pair<std::string, LetterStatus>>> userGuessesStatuses;

    std::string errorMessage;
    std::string finalMessage;

    std::string WIN_MESSAGE = "ПОБЕДА!";
    std::string LOSE_MESSAGE;

    GameStateDTO(int _maxAttempts, std::string _currentAlphabet,
                 std::string _wordToGuess) : isGameFinished(false), maxAttempts(_maxAttempts),
                                             targetWord(std::move(_wordToGuess)),
                                             currentAlphabet(std::move(_currentAlphabet)),
                                             LOSE_MESSAGE(std::format("Поражение. Слово было: {}", targetWord))
    {
        targetWordLength = countUnicodeCharacters(targetWord);
        targetWordByLetters = splitUtf8IntoCharacters(targetWord);
        std::vector<std::string> tempAlpha = splitUtf8IntoCharacters(currentAlphabet);
        for (const std::string &chr : tempAlpha)
        {
            currentAlphabetStatus[chr] = LetterStatus::IS_NOT_USED;
        }
    }

    GameStateDTO(GameStateDTO &&other) noexcept : isGameFinished(other.isGameFinished),
                                                  targetWordLength(other.targetWordLength),
                                                  maxAttempts(other.maxAttempts),
                                                  targetWord(std::move(other.targetWord)),
                                                  currentInputByLetters(std::move(other.currentInputByLetters)),
                                                  targetWordByLetters(std::move(other.targetWordByLetters)),
                                                  currentAlphabet(std::move(other.currentAlphabet)),
                                                  currentAlphabetStatus(std::move(other.currentAlphabetStatus)),
                                                  userGuesses(std::move(other.userGuesses)),
                                                  lastGuessStatus(std::move(other.lastGuessStatus)),
                                                  userGuessesStatuses(std::move(other.userGuessesStatuses)),
                                                  errorMessage(std::move(other.errorMessage)),
                                                  finalMessage(std::move(other.finalMessage)),
                                                  WIN_MESSAGE(other.WIN_MESSAGE),
                                                  LOSE_MESSAGE(other.LOSE_MESSAGE)
    {
    }

    GameStateDTO &operator=(GameStateDTO &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(isGameFinished, other.isGameFinished);
            std::swap(targetWordLength, other.targetWordLength);
            std::swap(maxAttempts, other.maxAttempts);

            std::swap(targetWord, other.targetWord);
            std::swap(currentInputByLetters, other.currentInputByLetters);
            std::swap(targetWordByLetters, other.targetWordByLetters);

            std::swap(currentAlphabet, other.currentAlphabet);
            std::swap(currentAlphabetStatus, other.currentAlphabetStatus);

            std::swap(userGuesses, other.userGuesses);
            std::swap(lastGuessStatus, other.lastGuessStatus);
            std::swap(userGuessesStatuses, other.userGuessesStatuses);

            std::swap(errorMessage, other.errorMessage);
            std::swap(finalMessage, other.finalMessage);

            std::swap(WIN_MESSAGE, other.WIN_MESSAGE);
            std::swap(LOSE_MESSAGE, other.LOSE_MESSAGE);
        }
        return *this;
    }

    void setCurrentInput(const std::string &input)
    {
        currentInputByLetters = splitUtf8IntoCharacters(input);
    }

    [[nodiscard]] const std::string getCurrentInputString() const
    {
        std::string res;
        for (const auto &let : currentInputByLetters)
        {
            res += let;
        }
        return res;
    }

    [[nodiscard]] const std::vector<std::string> getUserGuessByLetters(int index) const
    {
        return splitUtf8IntoCharacters(userGuesses[index]);
    }

    GameStateDTO(const GameStateDTO &other) = delete;

    GameStateDTO &operator=(const GameStateDTO &other) = delete;

private:
    // utils for utf8 strings

    static size_t countUnicodeCharacters(const std::string &utf8_string)
    {
        size_t count = 0;
        for (char c : utf8_string)
        {
            auto uc = static_cast<unsigned char>(c);

            if ((uc & 0xC0) != 0x80)
            {
                count++;
            }
        }
        return count;
    }

    static std::vector<std::string> splitUtf8IntoCharacters(const std::string &utf8_string)
    {
        std::vector<std::string> characters;
        if (utf8_string.empty())
        {
            return characters;
        }

        size_t startIndex = 0;

        for (size_t i = 0; i < utf8_string.length(); ++i)
        {
            auto currentByte = static_cast<unsigned char>(utf8_string[i]);

            bool isContinuationByte = (currentByte & 0xC0) == 0x80;

            if (i > 0 && !isContinuationByte)
            {
                std::string character = utf8_string.substr(startIndex, i - startIndex);
                characters.push_back(character);
                startIndex = i;
            }
        }

        if (startIndex < utf8_string.length())
        {
            std::string lastCharacter = utf8_string.substr(startIndex);
            characters.push_back(lastCharacter);
        }

        return characters;
    }
};