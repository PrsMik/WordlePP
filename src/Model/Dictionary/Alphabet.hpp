#pragma once

#include <map>
#include <string>

class Alphabet
{
public:
    enum class Language
    {
        ENGLISH,
        RUSSIAN
    };

private:
    static inline const std::map<Language, std::string> ALPHABETS{
        {Language::ENGLISH, "qwertyuiopasdfghjklzxcvbnm"},
        {Language::RUSSIAN, "йцукеёнгшщзхъфывапролджэячсмитьбю"}};

    static inline const std::map<Language, std::string> LANGUAGE_STRINGS{
        {Language::ENGLISH, "ENGLISH"},
        {Language::RUSSIAN, "RUSSIAN"}};

public:
    Alphabet() = delete;

    static std::string getAlphabet(Language _language)
    {
        return ALPHABETS.at(_language);
    }

    static std::string getLanguageString(Language _language)
    {
        return LANGUAGE_STRINGS.at(_language);
    }
};
