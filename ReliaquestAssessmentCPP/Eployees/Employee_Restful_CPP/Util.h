#pragma once

extern const wstring tok;

std::wstring& rtrim(std::wstring& s, const wstring t = tok);
std::wstring& ltrim(std::wstring& s, const wstring t = tok);
std::wstring parseKeyValue(const std::wstring& input, const std::wstring& key);

