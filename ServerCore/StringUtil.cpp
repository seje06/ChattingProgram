
#include "pch.h"
#include "StringUtil.h"

std::wstring Utf8ToWstring(const std::string& utf8str) {
    if (utf8str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8str[0], (int)utf8str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &utf8str[0], (int)utf8str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string WCHARToUTF8(const wchar_t* wstr) {
    if (wstr == nullptr) return "";

    // 1. 필요한 버퍼 크기 계산 (NULL 종료 문자 포함)
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);

    // 2. 출력용 string 생성 및 변환
    std::string strTo(sizeNeeded - 1, 0); // -1은 마지막 NULL 제외
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &strTo[0], sizeNeeded, NULL, NULL);

    return strTo;
}