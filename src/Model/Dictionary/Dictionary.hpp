#pragma once

#include <filesystem>
#include <fstream>
#include <string>

#include "IDictionary.hpp"

class Dictionary : public IDictionary
{
private:
    std::filesystem::path pathToDictionary;
    std::ifstream dictionaryFile;
    int amountOfWords;

public:
    Dictionary();

    Dictionary(Alphabet::Language lang, int wordLength);

    std::string getRandomWord() override;

    bool isValidWord(const std::string &) override;

    void setLanguageAndWordLength(Alphabet::Language lang, int wordLength) override;

    ~Dictionary();

    Dictionary(Dictionary &&other) noexcept;

    Dictionary &operator=(Dictionary &&other) noexcept;

    Dictionary(const Dictionary &other) = delete;

    Dictionary &operator=(const Dictionary &other) = delete;
};