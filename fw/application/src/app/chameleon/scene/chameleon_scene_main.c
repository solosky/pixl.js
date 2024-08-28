#include "app_chameleon.h"
#include "chameleon_scene.h"
#include "chameleon_view.h"
#include "amiibo_helper.h"

#include "nrf_log.h"

#include "settings2.h"
#include "hal_nfc_t2t.h"
#include "nfc_mf1.h"
#include "tag_emulation.h"

static void chameleon_scene_main_event_cb(chameleon_view_event_t event, chameleon_view_t *p_view) {
    app_chameleon_t *app = p_view->user_data;
    if (event == CHAMELEON_VIEW_EVENT_MENU) {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU);
    } else if (event == CHAMELEON_VIEW_EVENT_PREV) {
        uint8_t slot = tag_emulation_get_slot();
        slot = tag_emulation_slot_find_prev(slot);
        tag_emulation_change_slot(slot, true);
    } else if (event == CHAMELEON_VIEW_EVENT_NEXT) {
        uint8_t slot = tag_emulation_get_slot();
        slot = tag_emulation_slot_find_next(slot);
        tag_emulation_change_slot(slot, true);
    }
}

void chameleon_scene_main_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;

    hal_nfc_set_nrfx_irq_enable(true);

    tag_emulation_sense_run();

    chameleon_view_set_event_cb(app->p_chameleon_view, chameleon_scene_main_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_MAIN);
}

void chameleon_scene_main_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    tag_emulation_sense_end();
    hal_nfc_set_nrfx_irq_enable(false);
}