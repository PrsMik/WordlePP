#pragma once

#include "SDL3/SDL_stdinc.h"
#include <algorithm>
#include <locale>
#include <stdexcept>
#include <string>
#include <vector>

#include <codecvt>

// util to split utf-8 string into utf-8 charachters
static std::vector<std::string> splitUtf8String(const std::string &word)
{
    std::vector<std::string> chars;
    std::string tempWord = word;

    while (!tempWord.empty())
    {
        const char *fullStringCStr = tempWord.c_str();
        const char *endPtr = fullStringCStr + tempWord.length();
        const char *backPtr = endPtr;

        SDL_StepBackUTF8(fullStringCStr, &backPtr);

        if (backPtr != fullStringCStr)
        {
            size_t startIndex = backPtr - fullStringCStr;
            size_t charLength = endPtr - backPtr;

            std::string lastChar = tempWord.substr(startIndex, charLength);
            chars.push_back(lastChar);

            tempWord.erase(startIndex);
        }
        else
        {
            chars.push_back(tempWord);
            tempWord.clear();
        }
    }

    std::ranges::reverse(chars);

    return chars;
}

// util to change register of utf-8 string
static std::string utf8ToUpper(const std::string &utf8_str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    std::wstring wideStr;
    try
    {
        wideStr = converter.from_bytes(utf8_str);
    }
    catch (const std::range_error &e)
    {
        return utf8_str;
    }
    const std::locale &loc = std::locale("");
    const std::ctype<wchar_t> &ctype_facet = std::use_facet<std::ctype<wchar_t>>(loc);

    std::ranges::transform(
        wideStr,
        wideStr.begin(),
        [&ctype_facet](wchar_t chr)
        {
            return ctype_facet.toupper(chr);
        });

    std::string upperUtf8Str;
    try
    {
        upperUtf8Str = converter.to_bytes(wideStr);
    }
    catch (const std::range_error &e)
    {
        return "";
    }

    return upperUtf8Str;
}
