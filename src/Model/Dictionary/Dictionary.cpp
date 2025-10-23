#include "Dictionary.hpp"
#include "Alphabet.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <random>
#include <string>

Dictionary::Dictionary(Alphabet::Language lang, int wordLength) : pathToDictionary(DATA_DIR "/dictionaries/" +
                                                                                   Alphabet::getLanguageString(lang) +
                                                                                   std::format("_DICTIONARY_{}L.txt", wordLength)),
                                                                  dictionaryFile(pathToDictionary), amountOfWords(0)
{
    std::string tempString;

    if (dictionaryFile.is_open())
    {
        while (std::getline(dictionaryFile, tempString))
        {
            amountOfWords++;
        }
        dictionaryFile.close();
    }
    else
    {
        throw std::runtime_error("Не удалось открыть файл словаря!");
    }
}

std::string Dictionary::getRandomWord()
{
    std::random_device rndSeed;
    std::mt19937 rndGenerator(rndSeed());

    std::uniform_int_distribution<> randomIntNumber(0, amountOfWords);

    int currentLine = 0;
    int chosenLine = randomIntNumber(rndGenerator);
    std::string chosenWord;

    dictionaryFile.open(pathToDictionary);

    if (dictionaryFile.is_open())
    {
        while (std::getline(dictionaryFile, chosenWord))
        {
            if (currentLine == chosenLine)
            {
                break;
            }
            currentLine++;
        }
        dictionaryFile.close();
    }
    else
    {
        throw std::runtime_error("Не удалось открыть файл словаря!");
    }

    return chosenWord;
}

bool Dictionary::isValidWord(const std::string &inputWord)
{
    std::string currentWord;
    dictionaryFile.open(pathToDictionary);
    if (dictionaryFile.is_open())
    {
        while (std::getline(dictionaryFile, currentWord))
        {
            if (currentWord == inputWord)
            {
                dictionaryFile.close();
                return true;
            }
        }
        dictionaryFile.close();
    }
    else
    {
        throw std::runtime_error("Не удалось открыть файл словаря!");
    }
    return false;
}

void Dictionary::setLanguageAndWordLength(Alphabet::Language lang, int wordLength)
{
    pathToDictionary = DATA_DIR "/" + Alphabet::getLanguageString(lang) + std::format("_DICTIONARY_{}L.txt", wordLength);
    amountOfWords = 0;
    std::string tempString;

    dictionaryFile.open(pathToDictionary);

    if (dictionaryFile.is_open())
    {
        while (std::getline(dictionaryFile, tempString))
        {
            amountOfWords++;
        }
        dictionaryFile.close();
    }
    else
    {
        throw std::runtime_error("Не удалось открыть файл словаря!");
    }
}

Dictionary::~Dictionary()
{
    if (dictionaryFile.is_open())
    {
        dictionaryFile.close();
    }
}

Dictionary::Dictionary(Dictionary &&other) noexcept : pathToDictionary(std::move(other.pathToDictionary)),
                                                      dictionaryFile(std::move(other.dictionaryFile)),
                                                      amountOfWords(other.amountOfWords)
{
    other.amountOfWords = 0;
}

Dictionary &Dictionary::operator=(Dictionary &&other) noexcept
{
    if (this != &other)
    {
        std::swap(pathToDictionary, other.pathToDictionary);
        std::swap(dictionaryFile, other.dictionaryFile);

        amountOfWords = other.amountOfWords;
        other.amountOfWords = 0;
    }
    return *this;
}