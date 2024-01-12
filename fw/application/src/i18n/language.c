
#include "language.h"

typedef struct {
    const char **strings;
} LanguageData;

const LanguageData const languageData[LANGUAGE_COUNT] = {
    [LANGUAGE_ZH_HANS] = {.strings = lang_zh_Hans}, [LANGUAGE_EN_US] = {.strings = lang_en_US},
    [LANGUAGE_ZH_TW] = {.strings = lang_zh_TW},     [LANGUAGE_ES_ES] = {.strings = lang_es_ES},
    [LANGUAGE_HU_HU] = {.strings = lang_hu_HU},     [LANGUAGE_DE_DE] = {.strings = lang_de_DE},
    [LANGUAGE_FR_FR] = {.strings = lang_fr_FR},     [LANGUAGE_NL_NL] = {.strings = lang_nl_NL},
    [LANGUAGE_PT_BR] = {.strings = lang_pt_BR},     [LANGUAGE_JA_JP] = {.strings = lang_ja_JP},
    [LANGUAGE_PT_PT] = {.strings = lang_pt_PT},
};

// 当前语言设置 (Current language setting)
Language currentLanguage = LANGUAGE_ZH_HANS;

const char *getLangString(L_StringID stringID) {
    if (stringID >= _L_COUNT) {
        return "@@STR@@";
    }
    if (currentLanguage >= LANGUAGE_COUNT) {
        return lang_en_US[stringID];
    }
    const char *string = languageData[currentLanguage].strings[stringID];
    return string && strlen(string) > 0 ? string : lang_en_US[stringID];
}

void setLanguage(Language lang) { currentLanguage = lang; }

const char *getLangDesc(Language lang) {
    if (lang == LANGUAGE_ZH_HANS) {
        return "简体中文";
    } else if (lang == LANGUAGE_EN_US) {
        return "English";
    } else if (lang == LANGUAGE_ZH_TW) {
        return "繁體中文(臺灣)";
    } else if (lang == LANGUAGE_ES_ES) {
        return "Español";
    } else if (lang == LANGUAGE_HU_HU) {
        return "Magyar";
    } else if (lang == LANGUAGE_DE_DE) {
        return "Deutsch";
    } else if (lang == LANGUAGE_FR_FR) {
        return "Français";
    } else if (lang == LANGUAGE_NL_NL) {
        return "Dutch (Nederlands)";
    } else if (lang == LANGUAGE_PT_BR) {
        return "Português(Brazil)";
    } else if (lang == LANGUAGE_JA_JP) {
        return "日本語";
    } else if (lang == LANGUAGE_PT_PT) {
        return "Português(Portugal)";
    } else {
        return "@@LANG@@";
    }
}

Language getLanguage() { return currentLanguage; }