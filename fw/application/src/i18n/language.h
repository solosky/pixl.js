#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "string_id.h"

#define _T(x) getLangString(_L_##x)


typedef enum {
    LANGUAGE_ZH_HANS,
    LANGUAGE_EN_US,
    LANGUAGE_ZH_TW,
    LANGUAGE_ES_CO,
    LANGUAGE_COUNT
} Language;

extern const char* lang_en_US[_L_COUNT];
extern const char* lang_zh_Hans[_L_COUNT];
extern const char* lang_zh_TW[_L_COUNT];
extern const char* lang_es_CO[_L_COUNT];

// 获取字符串的函数
const char* getLangString(L_StringID stringID);
void setLanguage(Language lang);
Language getLanguage();
const char* getLangDesc(Language lang);


#endif // LANGUAGE_H
