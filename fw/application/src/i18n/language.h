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
    LANGUAGE_ES_ES,
    LANGUAGE_IT_IT,
    LANGUAGE_HU_HU,
    LANGUAGE_DE_DE,
    LANGUAGE_FR_FR,
    LANGUAGE_NL_NL,
    LANGUAGE_PT_BR,
    LANGUAGE_JA_JP,
    LANGUAGE_PT_PT,
    LANGUAGE_COUNT
} Language;

extern const char* lang_en_US[_L_COUNT];
extern const char* lang_zh_Hans[_L_COUNT];
extern const char* lang_zh_TW[_L_COUNT];
extern const char* lang_es_ES[_L_COUNT];
extern const char* lang_it_IT[_L_COUNT];
extern const char* lang_hu_HU[_L_COUNT];
extern const char* lang_de_DE[_L_COUNT];
extern const char* lang_fr_FR[_L_COUNT];
extern const char* lang_nl_NL[_L_COUNT];
extern const char* lang_pt_BR[_L_COUNT];
extern const char* lang_ja_JP[_L_COUNT];
extern const char* lang_pt_PT[_L_COUNT];


// 获取字符串的函数 (Get language string function)
const char* getLangString(L_StringID stringID);
void setLanguage(Language lang);
Language getLanguage();
const char* getLangDesc(Language lang);


#endif // LANGUAGE_H
