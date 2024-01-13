#include "app_chameleon.h"
#include "chameleon_scene.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "amiibo_helper.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "i18n/language.h"
#include "settings.h"

#include "mui_icons.h"
#include "tag_helper.h"

#include "app_status.h"
#include "bsp_time.h"
#include "rc522.h"

#include "boards.h"
static void tag_reader_enter() {
    nrf_gpio_pin_set(HF_ANT_SEL);
    nrf_gpio_pin_set(RD_PWR);
    bsp_timer_init();
    bsp_timer_start();
    pcd_14a_reader_init();
    pcd_14a_reader_reset();
}

static void tag_reader_exit() {
    nrf_gpio_pin_clear(HF_ANT_SEL);
    nrf_gpio_pin_clear(RD_PWR);
    bsp_timer_stop();
    bsp_timer_uninit();
    pcd_14a_reader_uninit();
}

void chameleon_scene_menu_card_read_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                             mui_list_item_t *p_item) {
    app_chameleon_t *app = p_list_view->user_data;
    char buff[16];
    switch (p_item->icon) {

    case ICON_NEW: {
        picc_14a_tag_t taginfo;
        uint8_t status = pcd_14a_reader_scan_auto(&taginfo);
        if (status == STATUS_HF_TAG_OK) {
            uint8_t buff[64];
            mui_list_view_clear_items(app->p_list_view);
            mui_list_view_add_item(app->p_list_view, ICON_NEW, "读取..", NULL_USER_DATA);
            mui_list_view_add_item(app->p_list_view, ICON_DATA, "CARD FOUND", NULL_USER_DATA);

            strcpy(buff, "[");
            tag_helper_format_uid(buff + 1, taginfo.uid, taginfo.uid_len);
            strcat(buff, "]");
            mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "UID", buff, NULL_USER_DATA);

            sprintf(buff, "[%02X]", taginfo.sak);
            mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "SAK", buff, NULL_USER_DATA);

            sprintf(buff, "[%02X %02X]", taginfo.atqa[1], taginfo.atqa[0]);
            mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "ATQA", buff, NULL_USER_DATA);

            mui_list_view_add_item(app->p_list_view, ICON_BACK, _T(MAIN_RETURN), NULL_USER_DATA);
        } else {
            mui_list_view_clear_items(app->p_list_view);
            mui_list_view_add_item(app->p_list_view, ICON_NEW, "读取..", NULL_USER_DATA);
            mui_list_view_add_item(app->p_list_view, ICON_DATA, "NO CARD", NULL_USER_DATA);
            mui_list_view_add_item(app->p_list_view, ICON_BACK, _T(MAIN_RETURN), NULL_USER_DATA);
        }
    } break;
    case ICON_DATA: {
        // int32_t slot = mui_list_view_get_focus(p_list_view) - 1;
        // tag_emulation_slot_set_enable(slot, TAG_SENSE_HF, !tag_emulation_slot_is_enabled(slot, TAG_SENSE_HF));
        // sprintf(buff, "[%s]", tag_emulation_slot_is_enabled(slot, TAG_SENSE_HF) ? _T(ON) : _T(OFF));
        // mui_list_view_item_set_sub_text(p_item, buff);
    } break;

    case ICON_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void chameleon_scene_menu_card_read_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;

    tag_reader_enter();

    mui_list_view_add_item(app->p_list_view, ICON_NEW, "读取..", NULL_USER_DATA);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, _T(MAIN_RETURN), NULL_USER_DATA);

    mui_list_view_set_selected_cb(app->p_list_view, chameleon_scene_menu_card_read_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST);
}

void chameleon_scene_menu_card_read_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);

    tag_reader_exit();
}