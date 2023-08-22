#ifndef MUI_TEXT_INPUT_H
#define MUI_TEXT_INPUT_H

#include "mui_mlib.h"
#include "m-array.h"
#include "m-string.h"
#include "mui_defines.h"
#include "mui_include.h"
#include <stdint.h>

#define ENTER_KEY '\r'
#define BACKSPACE_KEY '\b'

struct mui_text_input_s;
typedef struct mui_text_input_s mui_text_input_t;

typedef enum {
    MUI_TEXT_INPUT_EVENT_CONFIRMED,
    MUI_TEXT_INPUT_EVENT_CANCELLED,
} mui_text_input_event_t;

typedef void (*mui_text_input_event_cb_t)(mui_text_input_event_t event, mui_text_input_t *p_text_input);

struct mui_text_input_s {
    mui_view_t *p_view;
    uint8_t focus_row;
    uint8_t focus_column;
    string_t header;
    bool clear_default_text;
    string_t input_text;
    mui_text_input_event_cb_t event_cb;
    void *user_data;
};

mui_text_input_t *mui_text_input_create();
void mui_text_input_free(mui_text_input_t *p_view);
mui_view_t *mui_text_input_get_view(mui_text_input_t *p_view);
void mui_text_input_set_user_data(mui_text_input_t *p_view, void *user_data);

//// view functions //
void mui_text_input_set_event_cb(mui_text_input_t *p_view, mui_text_input_event_cb_t event_cb);
void mui_text_input_clear_input_text(mui_text_input_t *p_view);
void mui_text_input_set_input_text(mui_text_input_t *p_view, const char* input);
void mui_text_input_set_header(mui_text_input_t *p_view, const char* title);
const char* mui_text_input_get_input_text(mui_text_input_t *p_view);
void mui_text_input_reset(mui_text_input_t *p_view);
void mui_text_input_set_focus_key(mui_text_input_t *p_view, char key);

#endif