/*
 * ui_amiibo.c
 *
 *  Created on: 2021年10月4日
 *      Author: solos
 */

#include "lv_port_epd.h"
#include "ui_header.h"
#include "fonts.h"
#include "ui_amiibo.h"
#include "amiibo_data.h"
#include "nrf_log.h"
#include "ntag_indicator.h"
#include "ntag_store.h"
#include "ntag_emu.h"
#include "lvgl.h"
#include "ui.h"
#include "amiibo_helper.h"

#define AMIIBO_NAV_HEIGHT 25
#define AMIIBO_MAX_SLOTS  20 //TODO read from settings

static lv_obj_t *label_uid = NULL, *label_name = NULL, *label_game = NULL,
		*label_info = NULL;
static lv_style_t style_nav_focus;
static uint8_t amiibo_index = 0;
static lv_group_t *nav_group;
static lv_obj_t *cont, *nav_cont, *amiibo_cont;
static lv_obj_t *menu_list = NULL;
static bool created;

lv_obj_t* ui_amiibo_create(lv_obj_t *parent);

uint8_t ui_amiibo_get_index() {
	return amiibo_index;
}

static void nav_btn_key_event(lv_event_t *e) {
	ntag_t ntag;
	ret_code_t err_code;

	lv_event_code_t code = lv_event_get_code(e);
	NRF_LOG_INFO("btn event: code=%d, index=%d", code, e->user_data);
	switch (code) {
	case LV_EVENT_FOCUSED: {
		if (amiibo_index != ((uint32_t) e->user_data)) {
			amiibo_index = (uint32_t) e->user_data;
			err_code = ntag_store_read_default((uint8_t) amiibo_index, &ntag);
			APP_ERROR_CHECK(err_code);
			ntag_emu_set_tag(&ntag);
			ui_amiibo_set_ntag(&ntag);
			NRF_LOG_INFO("switch to tag: %d", amiibo_index);
		}
	}
		break;
	case LV_EVENT_LONG_PRESSED: {
		ui_switch_page(PAGE_MAIN);

	}
		break;

	case LV_EVENT_CLICKED: {
		NRF_LOG_INFO("amiibo: clicked");
		ui_amiibo_menu_create();
	}
		break;
	}

}

lv_obj_t* ui_amiibo_create(lv_obj_t *parent) {
	cont = lv_obj_create(parent);
	lv_obj_set_size(cont, LV_PCT(100), MY_DISP_VER_RES - STATUS_BAR_HEIGHT);
	lv_obj_set_align(cont, LV_ALIGN_BOTTOM_MID);
	lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_style_pad_top(cont, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_bottom(cont, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_left(cont, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_right(cont, 0, LV_PART_MAIN);
	lv_obj_set_style_border_width(cont, 0, LV_PART_MAIN);

	amiibo_cont = lv_obj_create(cont);
	lv_obj_set_size(amiibo_cont, LV_PCT(100),
	MY_DISP_VER_RES - STATUS_BAR_HEIGHT - AMIIBO_NAV_HEIGHT);
	lv_obj_set_align(amiibo_cont, LV_ALIGN_TOP_MID);
	lv_obj_set_scrollbar_mode(amiibo_cont, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_style_pad_top(amiibo_cont, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_bottom(amiibo_cont, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_left(amiibo_cont, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_right(amiibo_cont, 0, LV_PART_MAIN);

	static lv_coord_t col_dsc[] = { LV_GRID_CONTENT, LV_GRID_FR(1),
	LV_GRID_TEMPLATE_LAST };
	static lv_coord_t row_dsc[] = { 30, 20, 20, 20,
	LV_GRID_TEMPLATE_LAST };

	lv_obj_set_grid_dsc_array(amiibo_cont, col_dsc, row_dsc);
	lv_obj_set_layout(amiibo_cont, LV_LAYOUT_GRID);

	//static lv_style_t style_head_font;
	//lv_style_init(&style_head_font);
	//lv_style_set_text_font(&style_head_font, &lv_font_montserrat_18);

	label_uid = lv_label_create(amiibo_cont);
	lv_label_set_text(label_uid, "-1- 00:00:00:00:00:00");
	//lv_obj_add_style(label_uid, &style_head_font,
	//		LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_grid_cell(label_uid, LV_GRID_ALIGN_START, 0, 1,
			LV_GRID_ALIGN_CENTER, 0, 1);

	label_name = lv_label_create(amiibo_cont);
	lv_label_set_text(label_name, "Zelda");
	lv_obj_set_grid_cell(label_name, LV_GRID_ALIGN_START, 0, 1,
			LV_GRID_ALIGN_CENTER, 1, 1);

	label_game = lv_label_create(amiibo_cont);
	lv_label_set_text(label_game, "Legend Of Zelda");
	lv_obj_set_grid_cell(label_game, LV_GRID_ALIGN_START, 0, 1,
			LV_GRID_ALIGN_CENTER, 2, 1);

	label_info = lv_label_create(amiibo_cont);
	lv_label_set_text(label_info, "Legend Of Zelda");
	lv_obj_set_grid_cell(label_info, LV_GRID_ALIGN_START, 0, 1,
			LV_GRID_ALIGN_CENTER, 3, 1);

	nav_cont = lv_obj_create(cont);
	lv_obj_set_size(nav_cont, LV_PCT(100), AMIIBO_NAV_HEIGHT);
	lv_obj_set_align(nav_cont, LV_ALIGN_TOP_MID);
	lv_obj_set_scroll_snap_x(nav_cont, LV_SCROLL_SNAP_CENTER);
	lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_flex_flow(nav_cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_align(nav_cont, LV_ALIGN_BOTTOM_MID);
	lv_obj_set_style_pad_top(nav_cont, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_bottom(nav_cont, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_left(nav_cont, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_right(nav_cont, 0, LV_PART_MAIN);

	lv_style_init(&style_nav_focus);
	lv_style_set_bg_color(&style_nav_focus, lv_color_black());
	lv_style_set_text_color(&style_nav_focus, lv_color_white());
	//lv_style_set_bg_opa(&style_nav_focus, LV_OPA_50);
	lv_style_set_border_width(&style_nav_focus, 0);

	//nav_group = lv_group_create();
	//lv_indev_set_group(lv_port_default_indev(), nav_group);
	//lv_group_set_refocus_policy(nav_group, LV_GROUP_REFOCUS_POLICY_PREV);

	for (uint32_t i = 0; i < AMIIBO_MAX_SLOTS; i++) {
		lv_obj_t *btn1 = lv_btn_create(nav_cont);
		lv_obj_set_size(btn1, 20, 20);
		lv_obj_set_style_pad_top(btn1, 0, LV_PART_MAIN);
		lv_obj_set_style_pad_bottom(btn1, 0, LV_PART_MAIN);
		lv_obj_set_style_border_width(btn1, 0, LV_PART_MAIN);

		//lv_obj_add_event_cb(btn1, nav_btn_key_event, LV_EVENT_LONG_PRESSED, i);
		lv_obj_add_event_cb(btn1, nav_btn_key_event, LV_EVENT_LONG_PRESSED, i);
		lv_obj_add_event_cb(btn1, nav_btn_key_event, LV_EVENT_FOCUSED, i);
		lv_obj_add_event_cb(btn1, nav_btn_key_event, LV_EVENT_CLICKED, i);
		lv_obj_add_style(btn1, &style_nav_focus, LV_STATE_FOCUSED);
		lv_obj_t *label1 = lv_label_create(btn1);
		lv_label_set_text_fmt(label1, "%d", i + 1);
		lv_obj_center(label1);

		//lv_group_add_obj(nav_group, btn1);

		//if (i == amiibo_index) {
		//	lv_group_focus_obj(btn1);
		//}
	}

	// add callback
	/*for (uint32_t i = 0; i < lv_obj_get_child_cnt(nav_cont); i++) {
	 lv_obj_t *btn = lv_obj_get_child(nav_cont, i);
	 lv_obj_add_event_cb(btn, nav_btn_key_event, LV_EVENT_ALL, i);
	 if (i == amiibo_index) {
	 lv_group_focus_obj(btn);
	 }
	 }*/

	lv_obj_t *btn = lv_obj_get_child(nav_cont, 0);
	lv_group_focus_obj(btn);

	//setup emu
	ntag_t ntag;
	ret_code_t err_code;

	err_code = ntag_store_read_default((uint8_t) amiibo_index, &ntag);
	APP_ERROR_CHECK(err_code);
	err_code = ntag_emu_init(&ntag);
	ui_amiibo_set_ntag(&ntag);
	ui_header_show_status(ST_NFC);

    //load keys
    extern const uint8_t amiibo_key_retail[];
    amiibo_helper_load_keys(amiibo_key_retail);

	created = true;

	return cont;
}

void ui_amiibo_delete(lv_obj_t *page) {
	ntag_emu_uninit();
	ui_header_hide_status(ST_NFC);
}

void ui_amiibo_set_ntag(ntag_t *ntag) {

	if (label_uid == NULL) {
		return;
	}

	uint8_t uid1[7];
	uid1[0] = ntag->data[0];
	uid1[1] = ntag->data[1];
	uid1[2] = ntag->data[2];
	uid1[3] = ntag->data[4];
	uid1[4] = ntag->data[5];
	uid1[5] = ntag->data[6];
	uid1[6] = ntag->data[7];

	if (ntag->index != ui_amiibo_get_index()) {
		NRF_LOG_WARNING("tag may be corrupted.(%d, %d)", ntag->index,
				ui_amiibo_get_index());
	}

	lv_label_set_text_fmt(label_uid, "%02x:%02x:%02x:%02x:%02x:%02x:%02x",
			ntag->data[0], ntag->data[1], ntag->data[2], ntag->data[4],
			ntag->data[5], ntag->data[6], ntag->data[7]);

	uint32_t head = to_little_endian_int32(&ntag->data[84]);
	uint32_t tail = to_little_endian_int32(&ntag->data[88]);

	const amiibo_data_t *amd = find_amiibo_data(head, tail);
	if (amd != NULL) {
		lv_label_set_text(label_name, amd->name);
		lv_label_set_text(label_game, amd->game_series);
	} else {

		if (ntag->data[84] != 0 && ntag->data[85] != 0) {
			lv_label_set_text_fmt(label_name,
					"%02x%02x%02x%02x %02x%02x%02x%02x", ntag->data[84],
					ntag->data[85], ntag->data[86], ntag->data[87],
					ntag->data[88], ntag->data[89], ntag->data[90],
					ntag->data[91]);
			lv_label_set_text(label_game, "<unknown amiibo>");
		} else {
			lv_label_set_text(label_name, "<empty amiibo>");
			lv_label_set_text(label_game, "");
		}
	}

	lv_label_set_text(label_info, "NOTE");

}

static void ui_amiibo_close_menu() {
	lv_obj_del_async(menu_list);
	lv_obj_t *btn = lv_obj_get_child(nav_cont, amiibo_index);
	lv_group_focus_obj(btn);
	menu_list = NULL;
}

static void menu_item_reset_uuid_clicked_event(lv_event_t *e) {
	ret_code_t err_code;
	ntag_t ntag_new;
	ntag_t *ntag_current = ntag_emu_get_current_tag();
	memcpy(&ntag_new, ntag_current, sizeof(ntag_t));

    NRF_LOG_INFO("reset uuid begin");

	err_code = ntag_store_uuid_rand(&ntag_new);
    APP_ERROR_CHECK(err_code);


    //sign new
    err_code = amiibo_helper_sign_new_ntag(ntag_current, &ntag_new);
    APP_ERROR_CHECK(err_code);

	if (err_code == NRF_SUCCESS) {
		//ntag_emu_set_uuid_only(&ntag_new);
		ntag_emu_set_tag(&ntag_new);
		ui_amiibo_set_ntag(&ntag_new);

		NRF_LOG_INFO("reset uuid success");
	}

	ui_amiibo_close_menu();

}

static void menu_item_reset_ntag_clicked_event(lv_event_t *e) {
	ntag_t ntag;
	ret_code_t err_code;
	ntag_store_generate(ui_amiibo_get_index(), &ntag);
	err_code = ntag_store_write_with_gc(ui_amiibo_get_index(), &ntag);
	if (err_code == NRF_SUCCESS) {
		ntag_emu_set_tag(&ntag);
		ui_amiibo_set_ntag(&ntag);
	} else {
		NRF_LOG_INFO("store write failed: %d", err_code);
	}
	ui_amiibo_close_menu();
}

static void menu_item_close_menu_clicked_event(lv_event_t *e) {
	ui_amiibo_close_menu();
}

static void menu_item_switch_main_clicked_event(lv_event_t *e) {
	ui_switch_page(PAGE_MAIN);
	ui_amiibo_close_menu();
}

void ui_amiibo_menu_create() {
	if (menu_list != NULL) {
		return;
	}
	/*Create a list*/
	menu_list = lv_list_create(cont);
	lv_obj_set_size(menu_list, 120, LV_PCT(100));
	lv_obj_set_align(menu_list, LV_ALIGN_RIGHT_MID);
	//lv_obj_add_flag(menu_list, LV_OBJ_FLAG_FLOATING);
	//lv_obj_align(menu_list, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
	//lv_obj_set_pos(menu_list, 80, 0);

	lv_obj_t *btn;

	btn = lv_list_add_btn(menu_list, LV_SYMBOL_REFRESH, "Gen. UID");
	lv_obj_add_event_cb(btn, menu_item_reset_uuid_clicked_event,
			LV_EVENT_CLICKED, NULL);
	lv_group_focus_obj(btn);
	btn = lv_list_add_btn(menu_list, LV_SYMBOL_LOOP, "Reset");
	lv_obj_add_event_cb(btn, menu_item_reset_ntag_clicked_event,
			LV_EVENT_CLICKED, NULL);
	btn = lv_list_add_btn(menu_list, LV_SYMBOL_HOME, "Home");
	lv_obj_add_event_cb(btn, menu_item_switch_main_clicked_event,
			LV_EVENT_CLICKED, NULL);
	btn = lv_list_add_btn(menu_list, LV_SYMBOL_CLOSE, "Close");
	lv_obj_add_event_cb(btn, menu_item_close_menu_clicked_event,
			LV_EVENT_CLICKED, NULL);

	lv_obj_move_foreground(menu_list);
}
