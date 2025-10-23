#include "WordleModel.hpp"
#include "GameState.hpp"
#include <string>
#define NOMINMAX
#include <algorithm>
#include <format>
#include <memory>
#include <securitybaseapi.h>
#include <utility>

WordleModel::WordleModel(Alphabet::Language _alphabetLanguage,
                         std::unique_ptr<IDictionary> _gameDictionary,
                         int _maxAttempts) : gameLanguage(_alphabetLanguage),
                                             gameDictionary(std::move(_gameDictionary)),
                                             gameState(_maxAttempts, Alphabet::getAlphabet(gameLanguage),
                                                       gameDictionary->getRandomWord())
{
}

void WordleModel::startNewGame()
{
    gameState = GameStateDTO(gameState.maxAttempts,
                             gameState.currentAlphabet,
                             gameDictionary->getRandomWord());
}

void WordleModel::checkInputWord()
{
    std::string tempLastInput;
    for (auto &let : gameState.currentInputByLetters)
    {
        tempLastInput += let;
    }
    gameState.userGuesses.push_back(tempLastInput);
    gameState.lastGuessStatus.clear();
    gameState.lastGuessStatus.resize(gameState.targetWordLength);

    std::map<std::string, int> targetLetterCounts;
    for (const auto &letter : gameState.targetWordByLetters)
    {
        targetLetterCounts[letter]++;
    }

    for (int letterIndex = 0; letterIndex < gameState.currentInputByLetters.size(); ++letterIndex)
    {
        const std::string &currentLetter = gameState.currentInputByLetters[letterIndex];
        const std::string &targetLetter = gameState.targetWordByLetters[letterIndex];

        if (currentLetter == targetLetter)
        {
            gameState.lastGuessStatus[letterIndex] = {currentLetter,
                                                      GameStateDTO::LetterStatus::IS_IN_PLACE};
            targetLetterCounts[currentLetter]--;
        }
    }

    for (int letterIndex = 0; letterIndex < gameState.currentInputByLetters.size(); ++letterIndex)
    {
        if (gameState.lastGuessStatus[letterIndex].second == GameStateDTO::LetterStatus::IS_IN_PLACE)
        {
            gameState.currentAlphabetStatus[gameState.currentInputByLetters[letterIndex]] = std::max(
                gameState.lastGuessStatus[letterIndex].second,
                gameState.currentAlphabetStatus[gameState.currentInputByLetters[letterIndex]]);
            continue;
        }

        const std::string &currentLetter = gameState.currentInputByLetters[letterIndex];

        if (targetLetterCounts.contains(currentLetter) && targetLetterCounts[currentLetter] > 0)
        {
            gameState.lastGuessStatus[letterIndex] = {currentLetter,
                                                      GameStateDTO::LetterStatus::IS_IN_WORD_NOT_IN_PLACE};
            targetLetterCounts[currentLetter]--;
        }
        else
        {
            gameState.lastGuessStatus[letterIndex] = {currentLetter,
                                                      GameStateDTO::LetterStatus::IS_NOT_IN_WORD};
        }

        gameState.currentAlphabetStatus[currentLetter] = std::max(
            gameState.lastGuessStatus[letterIndex].second,
            gameState.currentAlphabetStatus[currentLetter]);
    }

    gameState.userGuessesStatuses.push_back(gameState.lastGuessStatus);
    if (isGameOver())
    {
        gameState.isGameFinished = true;
        gameState.finalMessage = isUserWin() ? gameState.WIN_MESSAGE : gameState.LOSE_MESSAGE;
    }
}

bool WordleModel::isValidInput()
{
    bool res = false;
    if (gameState.currentInputByLetters.size() == gameState.targetWordLength)
    {
        for (const auto &currentInputByLetter : gameState.currentInputByLetters)
        {
            if (!gameState.currentAlphabetStatus.contains(currentInputByLetter))
            {
                gameState.errorMessage = "Содержатся символы, не входящие в алфавит.";
                return res;
            }
        }
        if (!gameDictionary->isValidWord(gameState.getCurrentInputString()))
        {
            gameState.errorMessage = "Такого слова нет в словаре для текущего языка.";
            return res;
        }
        res = true;
    }
    else
    {
        gameState.errorMessage = std::format("Длина слова должна быть {} букв.", gameState.targetWordLength);
    }
    return res;
}

void WordleModel::modifyCurrentInput(const std::string &input)
{
    gameState.setCurrentInput(input);
}

bool WordleModel::isGameOver() const
{
    return gameState.userGuesses.size() == gameState.maxAttempts || this->isUserWin();
}

bool WordleModel::isUserWin() const
{
    if (gameState.userGuesses.empty())
    {
        return false;
    }
    return gameState.targetWord == gameState.userGuesses.back();
}

GameStateDTO &WordleModel::getGameState()
{
    return gameState;
}

WordleModel::~WordleModel() = default;

WordleModel::WordleModel(WordleModel &&other) noexcept : gameLanguage(other.gameLanguage),
                                                         gameDictionary(std::move(other.gameDictionary)),
                                                         gameState(std::move(other.gameState))
{
}

WordleModel &WordleModel::operator=(WordleModel &&other) noexcept
{
    if (this != &other)
    {
        std::swap(gameLanguage, other.gameLanguage);
        std::swap(gameDictionary, other.gameDictionary);
        std::swap(gameState, other.gameState);
    }
    return *this;
}