#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LANGUAGE_H
#define LANGUAGE_H

typedef struct {
    const char** strings;
} LanguageData;

typedef enum {
    LANGUAGE_ZH_HANS,
    LANGUAGE_EN_US,
    LANGUAGE_ZH_TW,
    LANGUAGE_JA_JP,
    LANGUAGE_KO_KR,
    LANGUAGE_COUNT
} Language;

typedef enum {
    _L_ON,
    _L_OFF,
    _L_ERR,
    _L_ERR_CODE,
    _L_NOT_AMIIBO_FILE,
    _L_READ_FILE_FAILED,
    _L_APP_AMIIBO,
    _L_APP_AMIIBOLINK,
    _L_APP_BLE,
    _L_APP_BLE_TITLE,
    _L_APP_PLAYER,
    _L_APP_SET,
    _L_APP_SET_VERSION,
    _L_APP_SET_SKIP_DRIVER_SELECT,
    _L_APP_SET_OLED_CONTRAST,
    _L_APP_SET_OLED_CONTRAST_TITLE,
    _L_APP_SET_LCD_BACKLIGHT,
    _L_APP_SET_LCD_BACKLIGHT_TITLE,
    _L_APP_SET_ANIM,
    _L_APP_SET_LIPO_BAT,
    _L_APP_SET_SHOW_MEM_USAGE,
    _L_APP_SET_HIBERNATE,
    _L_APP_SET_SLEEP_TIMEOUT,
    _L_APP_SET_LANGUAGE,
    _L_APP_SET_DFU,
    _L_APP_SET_MENU_EXIT,
    _L_15S,
    _L_30S,
    _L_45S,
    _L_1MIN,
    _L_2MIN,
    _L_3MIN,
    _L_BACK,
    _L_AMIIBO_KEY_UNLOADED,
    _L_UPLOAD_KEY_RETAIL_BIN,
    _L_KNOW,
    _L_RANDOM_GENERATION,
    _L_AUTO_RANDOM_GENERATION,
    _L_DELETE_TAG,
    _L_BACK_TO_DETAILS,
    _L_BACK_TO_FILE_LIST,
    _L_BACK_TO_MAIN_MENU,
    _L_FORMAT,
    _L_FORMAT_STORAGE,
    _L_DELETE_ALL_DATA,
    _L_CONFIRM,
    _L_CANCEL,
    _L_BACK_TO_LIST,
    _L_NOT_MOUNTED,
    _L_MOUNTED_LFS,
    _L_MOUNTED_FFS,
    _L_TOTAL_SPACE,
    _L_AVAILABLE_SPACE,
    _L_INPUT_FOLDER_NAME,
    _L_INPUT_AMIIBO_NAME,
    _L_DELETE,
    _L_INPUT_NEW_NAME,
    _L_CREATE_NEW_FOLDER,
    _L_CREATE_NEW_TAG,
    _L_RENAME,
    _L_MAIN_MENU,
    _L_OPEN_FOLDER_FAILED,
    _L_RENAME_FAILED,
    _L_MAIN_RETURN,
    _L_RANDOM_MODE_MANUAL,
    _L_RANDOM_MODE_AUTO,
    _L_SEQUENCE_MODE,
    _L_READ_WRITE_MODE,
    _L_AMIIBOLINK_V1,
    _L_AMIIBOLINK_V2,
    _L_AMILOOP,
    _L_MODE,
    _L_AUTO_RANDOM,
    _L_COMPATIBLE_MODE,
    _L_TAG_DETAILS,
    _L_MODE_RANDOM,
    _L_MODE_CYCLE,
    _L_MODE_NTAG,
    _L_MODE_RANDOM_AUTO_GEN,
    _L_BLANK_TAG,
    _L_COUNT  // 字符串的数量
} L_StringID;

extern const char* lang_en_us[_L_COUNT];
extern const char* lang_zh_hans[_L_COUNT];
extern const char* lang_zh_tw[_L_COUNT];

// 获取字符串的函数
const char* getLangString(L_StringID stringID);
void setLanguage(Language lang);
const char* getLangDesc(Language lang);

#endif // LANGUAGE_H
