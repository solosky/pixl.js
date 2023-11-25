#include "app_chameleon.h"
#include "chameleon_scene.h"
#include "chameleon_view.h"
#include "i18n/language.h"
#include "tag_emulation.h"

#include "nrf_log.h"

#include "vfs.h"
#include "vfs_meta.h"

void chameleon_scene_factory_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;

    mui_msg_box_set_header(app->p_msg_box, _T(MESSAGE));
    mui_msg_box_set_message(app->p_msg_box, _T(APP_CHAMELEON_INITIALIZING));
    mui_msg_box_set_btn_text(app->p_msg_box, NULL, NULL, NULL);
    mui_msg_box_set_btn_focus(app->p_msg_box, 0);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_MSG_BOX);

    mui_update_now(mui());

    vfs_driver_t *p_driver = vfs_get_default_driver();

    p_driver->create_dir("/chameleon");
    p_driver->create_dir("/chameleon/slots");
    p_driver->create_dir("/chameleon/dump");

    vfs_meta_t meta = {0};
    uint8_t buff[VFS_META_MAX_SIZE];
    meta.has_flags = true;
    meta.flags = VFS_OBJ_FLAG_HIDDEN;
    vfs_meta_encode(buff,sizeof(buff), &meta);
    p_driver->update_file_meta("/chameleon", buff, sizeof(buff));


    tag_emulation_factory_init();

    tag_emulation_change_slot(0, false);

    mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MAIN);
}

void chameleon_scene_factory_on_exit(void *user_data) {}