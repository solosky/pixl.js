#include "mui_canvas.h"


void mui_canvas_flush(mui_canvas_t* p_canvas){
     u8g2_SendBuffer(p_canvas->fb);
}

void mui_canvas_clear(mui_canvas_t* p_canvas){
     u8g2_ClearBuffer(p_canvas->fb);
}

void mui_canvas_set_font(mui_canvas_t* p_canvas, const uint8_t  *font){
    u8g2_SetFont(p_canvas->fb, font);
}


void mui_canvas_draw_utf8(mui_canvas_t* p_canvas, uint8_t x, uint8_t y, const char* str){
    if(!str) return;
    x += p_canvas->offset_x;
    y += p_canvas->offset_y;
    u8g2_DrawUTF8(p_canvas->fb, x, y,  str);
}