#include "messages.h"
#include <stddef.h>

static enum message_languages_t current_language = MESSAGE_LANG_ZH_CN;
const enum message_languages_t default_language = MESSAGE_LANG_ZH_CN;

const char* get_message_in_language(enum messages_t message_id, enum message_languages_t language_id) {
    switch(language_id) {
                case MESSAGE_LANG_ZH_CN:
            switch(message_id) {
                case MESSAGE_ID_1_MINUTE:
                    return "1分钟";
                case MESSAGE_ID_15_SECONDS:
                    return "15秒";
                case MESSAGE_ID_2_MINUTES:
                    return "2分钟";
                case MESSAGE_ID_3_MINUTES:
                    return "3分钟";
                case MESSAGE_ID_30_SECONDS:
                    return "30秒";
                case MESSAGE_ID_45_SECONDS:
                    return "45秒";
                case MESSAGE_ID_AUTO_SELECT_STORAGE_ARG_STRING:
                    return "自动选择存储 [%s]";
                case MESSAGE_ID_BACK:
                    return "返回";
                case MESSAGE_ID_BACK_TO_MAIN_MENU:
                    return "返回主菜单";
                case MESSAGE_ID_BACKLIGHT_BRIGHTNESS:
                    return "背光亮度";
                case MESSAGE_ID_BACKLIGHT_OFF:
                    return "背光亮度 [关]";
                case MESSAGE_ID_BACKLIGHT_ON_ARG_INT:
                    return "背光亮度 [%d%%]";
                case MESSAGE_ID_BRANCH_ARG_STRING:
                    return "Branch [%s]";
                case MESSAGE_ID_BUILD_DATE_ARG_STRING:
                    return "BuildDate [%s]";
                case MESSAGE_ID_DIRTY_ARG_INT:
                    return "Dirty [%d]";
                case MESSAGE_ID_GIT_HASH_ARG_STRING:
                    return "GitHash [%s]";
                case MESSAGE_ID_LANGUAGE_ARG_STRING:
                    return "Language [%s]";
                case MESSAGE_ID_LIPO_BATTERY_ARG_STRING:
                    return "LiPO电池 [%s]";
                case MESSAGE_ID_MEMORY_USED_ARG_STRING:
                    return "内存使用率 [%s]";
                case MESSAGE_ID_OFF:
                    return "关";
                case MESSAGE_ID_ON:
                    return "开";
                case MESSAGE_ID_QUICK_WAKE_ARG_STRING:
                    return "快速唤醒 [%s]";
                case MESSAGE_ID_SLEEP_TIMER_ARG_INT:
                    return "休眠时间 [%ds]";
                case MESSAGE_ID_SYSTEM_SETTINGS:
                    return "系统设置";
                case MESSAGE_ID_TARGET_ARG_INT:
                    return "Target [%d]";
                case MESSAGE_ID_UPDATE_FIRMWARE:
                    return "固件更新";
                case MESSAGE_ID_VERSION_ARG_STRING:
                    return "版本 [%s]";
                default:
                    // This is the default language so this should return something
                    return "失踪";
            }
        case MESSAGE_LANG_EN_US:
            switch(message_id) {
                case MESSAGE_ID_DEFAULT_STRING:
                    return "MISSING";
                case MESSAGE_ID_1_MINUTE:
                    return "1 minute";
                case MESSAGE_ID_15_SECONDS:
                    return "15 seconds";
                case MESSAGE_ID_2_MINUTES:
                    return "2 minutes";
                case MESSAGE_ID_3_MINUTES:
                    return "3 minutes";
                case MESSAGE_ID_30_SECONDS:
                    return "30 seconds";
                case MESSAGE_ID_45_SECONDS:
                    return "45 seconds";
                case MESSAGE_ID_AUTO_SELECT_STORAGE_ARG_STRING:
                    return "Auto Select Storage [%s]";
                case MESSAGE_ID_BACK:
                    return "Back";
                case MESSAGE_ID_BACK_TO_MAIN_MENU:
                    return "Back to Main Menu";
                case MESSAGE_ID_BACKLIGHT_BRIGHTNESS:
                    return "Backlight Brightness";
                case MESSAGE_ID_BACKLIGHT_OFF:
                    return "Backlight [Off]";
                case MESSAGE_ID_BACKLIGHT_ON_ARG_INT:
                    return "Backlight [%d%%]";
                case MESSAGE_ID_BRANCH_ARG_STRING:
                    return "Branch [%s]";
                case MESSAGE_ID_BUILD_DATE_ARG_STRING:
                    return "BuildDate [%s]";
                case MESSAGE_ID_DIRTY_ARG_INT:
                    return "Dirty [%d]";
                case MESSAGE_ID_GIT_HASH_ARG_STRING:
                    return "GitHash [%s]";
                case MESSAGE_ID_LANGUAGE_ARG_STRING:
                    return "Language";
                case MESSAGE_ID_LIPO_BATTERY_ARG_STRING:
                    return "LiPO Battery [%s]";
                case MESSAGE_ID_MEMORY_USED_ARG_STRING:
                    return "Memory Used [%s]";
                case MESSAGE_ID_OFF:
                    return "Off";
                case MESSAGE_ID_ON:
                    return "On";
                case MESSAGE_ID_QUICK_WAKE_ARG_STRING:
                    return "Quick Wake [%s]";
                case MESSAGE_ID_SLEEP_TIMER_ARG_INT:
                    return "Sleep Timer [%ds]";
                case MESSAGE_ID_SYSTEM_SETTINGS:
                    return "System Settings";
                case MESSAGE_ID_TARGET_ARG_INT:
                    return "Target [%d]";
                case MESSAGE_ID_UPDATE_FIRMWARE:
                    return "Update Firmware";
                case MESSAGE_ID_VERSION_ARG_STRING:
                    return "Version [%s]";
                default:
                    break;
            }
        default:
            break;
    }
    return NULL;
}

const char* get_message(enum messages_t message_id) {
    const char* message = get_message_in_language(message_id, current_language);
    if (message == NULL) {
        return get_message_in_language(message_id, default_language);
    }
}

void set_messages_language(enum message_languages_t language_id){
    current_language = language_id;
}
