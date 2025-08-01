
#include "stdafx.h"

const wstring tok = L"{}";

// trim from end of string (right)
std::wstring& rtrim(std::wstring& s, const wstring t)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
std::wstring& ltrim(std::wstring& s, const wstring t)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

std::wstring parseKeyValue(const std::wstring& input, const std::wstring& key) {
    size_t pos = input.find(key);
    if (pos != std::string::npos)
        return input.substr(pos + key.length());
    else
        return L"";
}

