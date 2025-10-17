#pragma once

#include "Alphabet.hpp"
#include <string>

class IDictionary
{
public:
    virtual std::string getRandomWord() = 0;

    virtual bool isValidWord(const std::string &) = 0;

    virtual void setLanguageAndWordLength(Alphabet::Language, int) = 0;

    virtual ~IDictionary() = default;
};
