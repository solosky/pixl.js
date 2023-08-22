
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

void setLanguage(Language lang) {
   currentLanguage = lang;
}

const char* getLangDesc(Language lang){
    if(lang == LANGUAGE_ZH_HANS){
        return "简体中文";
    }else if(lang == LANGUAGE_EN_US){
        return "English";
    }else if(lang == LANGUAGE_ZH_TW){
        return "繁體中文(臺灣)";
    }
}

Language getLanguage() {
    return currentLanguage;
}