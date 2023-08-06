
#include "language.h"

LanguageData languageData[LANGUAGE_COUNT] = {
    [LANGUAGE_ZH_HANS] = { .strings = lang_zh_hans },
    [LANGUAGE_EN_US] = { .strings = lang_en_us },
    [LANGUAGE_ZH_TW] = { .strings = lang_zh_tw },
};

// 当前语言设置
Language currentLanguage = LANGUAGE_ZH_HANS;


const char* getLangString(L_StringID stringID) {
    const char* string = languageData[currentLanguage].strings[stringID];
    return string ? string : lang_zh_hans[stringID];  // 如果没有翻译，使用中文
}

void setLanguage(char* language) {
    if (strcmp(language, "zh_Hans") == 0) {
        currentLanguage = LANGUAGE_ZH_HANS;
    } else if (strcmp(language, "en_US") == 0) {
        currentLanguage = LANGUAGE_EN_US;
    }else if (strcmp(language, "zh_TW") == 0) {
        currentLanguage = LANGUAGE_ZH_TW;
    }else if (strcmp(language, "ja_JP") == 0) {
        currentLanguage = LANGUAGE_JA_JP;
    }else if (strcmp(language, "ko_KR") == 0) {
        currentLanguage = LANGUAGE_KO_KR;
    } else {
        currentLanguage = LANGUAGE_ZH_HANS; // 默认语言
    }
}