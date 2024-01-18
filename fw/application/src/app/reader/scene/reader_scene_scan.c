#include "reader_scene.h"

#include "app_reader.h"
#include "nfc_scan_view.h"

#include "app_scheduler.h"
#include "app_timer.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"
#include "nfc_reader.h"
#include "nrf_log.h"
#include "utils2.h"

APP_TIMER_DEF(m_scan_timer);



static void reader_scene_nfc_scan_execute(void *p_event_data, uint16_t event_size) {
    app_reader_t **p_app = p_event_data;
    app_reader_t *app = *p_app;
    uint32_t begin = app_timer_cnt_get();
    uint8_t status = pcd_14a_reader_scan_auto(&(app->tag_info));
    uint32_t end = app_timer_cnt_get();
    if (status == STATUS_HF_TAG_OK) {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, READER_SCENE_CARD_FOUND);
    } else {
        NRF_LOG_INFO("card not found: %lu ms", app_timer_ms(end - begin));
    }
}
static void reader_scene_nfc_scan_on_timer(void *p_context) {
    app_sched_event_put(&p_context, sizeof(&p_context), reader_scene_nfc_scan_execute);
}

void reader_scene_nfc_scan_view_event_cb(nfc_scan_view_event_t event, nfc_scan_view_t *p_view) {
    app_reader_t *app = p_view->user_data;
    mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, READER_SCENE_SCAN_MENU);
}

void reader_scene_scan_on_enter(void *user_data) {
    app_reader_t *app = user_data;
    ret_code_t err_code;

    nfc_reader_enter();

    err_code = app_timer_create(&m_scan_timer, APP_TIMER_MODE_REPEATED, reader_scene_nfc_scan_on_timer);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_scan_timer, APP_TIMER_TICKS(1200), app);
    APP_ERROR_CHECK(err_code);

    nfc_scan_view_set_event_cb(app->p_nfc_scan_view, reader_scene_nfc_scan_view_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, READER_VIEW_ID_SCAN);
}

void reader_scene_scan_on_exit(void *user_data) {
    nfc_reader_exit();
    app_timer_stop(m_scan_timer);
    app_reader_t *app = user_data;
}