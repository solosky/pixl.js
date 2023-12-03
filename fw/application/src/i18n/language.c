
#include "language.h"


typedef struct {
    const char** strings;
} LanguageData;

LanguageData languageData[LANGUAGE_COUNT] = {
    [LANGUAGE_ZH_HANS] = { .strings = lang_zh_Hans },
    [LANGUAGE_EN_US] = { .strings = lang_en_US },
    [LANGUAGE_ZH_TW] = { .strings = lang_zh_TW },
    [LANGUAGE_ES_ES] = { .strings = lang_es_ES },
};

// 当前语言设置
Language currentLanguage = LANGUAGE_ZH_HANS;


const char* getLangString(L_StringID stringID) {
    const char* string = languageData[currentLanguage].strings[stringID];
    return string && strlen(string) > 0 ? string : lang_en_US[stringID];
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
    }else if(lang == LANGUAGE_ES_ES){
        return  "Español";
    }
}

Language getLanguage() {
    return currentLanguage;
}