#include "WordleModel.hpp"
#include "GameState.hpp"
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
                                             gameState(_maxAttempts, Alphabet::getLanguageString(gameLanguage),
                                                       gameDictionary->getRandomWord())
{
}

void WordleModel::startNewGame()
{
    for (char letter : gameState.currentAlphabet)
    {
        gameState.currentAlphabetStatus[letter] = GameStateDTO::LetterStatus::IS_NOT_USED;
    }
    gameState.targetWord = gameDictionary->getRandomWord();
}

void WordleModel::checkInputWord(const std::string &inputWord)
{
    gameState.userGuesses.push_back(inputWord);
    gameState.lastGuessStatus.clear();

    char letter;
    std::map<char, std::vector<int>> targetWordLetterIndexes;
    for (int index = 0; index < gameState.targetWordLength; ++index)
    {
        letter = gameState.targetWord[index];
        targetWordLetterIndexes[letter].push_back(index);
    }

    std::vector<int> possibleIndexes;
    for (int letterIndex = 0; letterIndex < inputWord.size(); ++letterIndex)
    {
        letter = inputWord[letterIndex];
        try
        {
            possibleIndexes = targetWordLetterIndexes.at(letter);
            auto iter = std::ranges::find(possibleIndexes, letterIndex);

            if (iter != possibleIndexes.end())
            {
                gameState.lastGuessStatus[letter] = GameStateDTO::LetterStatus::IS_IN_PLACE;
            }
            else
            {
                gameState.lastGuessStatus[letter] = GameStateDTO::LetterStatus::IS_IN_WORD_NOT_IN_PLACE;
            }

            possibleIndexes.erase(iter);
            if (possibleIndexes.empty())
            {
                targetWordLetterIndexes.erase(letter);
            }
        }
        catch (const std::exception &e)
        {
            gameState.lastGuessStatus[letter] = GameStateDTO::LetterStatus::IS_NOT_IN_WORD;
        }
        gameState.currentAlphabetStatus[letter] = std::max(gameState.lastGuessStatus[letter], gameState.currentAlphabetStatus[letter]);
    }
}

bool WordleModel::isValidInput(const std::string &inputWord)
{
    bool res = false;
    if (inputWord.size() == gameState.targetWordLength)
    {
        for (char letter : inputWord)
        {
            if (!gameState.currentAlphabetStatus.contains(letter))
            {
                gameState.errorMessage = "Содержатся символы, не входящие в алфавит.";
                return res;
            }
        }
        res = true;
    }
    else
    {
        gameState.errorMessage = std::format("Длина слова должна быть {} букв.", gameState.targetWordLength);
    }
    return res;
}

bool WordleModel::isGameOver() const
{
    return gameState.userGuesses.size() == gameState.maxAttempts || this->isUserWin();
}

bool WordleModel::isUserWin() const
{
    return gameState.targetWord == gameState.userGuesses.back();
}

const GameStateDTO &WordleModel::getGameState() const
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