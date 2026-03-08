
#include "pch.h"
#include "StringUtil.h"

std::wstring Utf8ToWstring(const std::string& utf8str) {
    if (utf8str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8str[0], (int)utf8str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &utf8str[0], (int)utf8str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
