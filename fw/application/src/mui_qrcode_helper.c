#include "mui_qrcode_helper.h"

// Prints the given QR Code to the console.
void printQr(const uint8_t qrcode[], mui_canvas_t *p_canvas, uint8_t x0, uint8_t y0, uint8_t maxsize) {
    uint8_t color = mui_canvas_get_draw_color(p_canvas);
    int size = qrcodegen_getSize(qrcode);
    int border = 4;
    int scale = maxsize / size;
    int scale_x = scale;
    int scale_y = scale;
    for (int y = -border; y < size + border; y++) {
        for (int x = -border; x < size + border; x++) {
            mui_canvas_set_draw_color(p_canvas, qrcodegen_getModule(qrcode, x, y) ? 1 : 0);
            if (x < 0 || x >= size) {
                scale_x = 1;
            } else {
                scale_x = scale;
            }
            if (y < 0 || y >= size) {
                scale_y = 1;
            } else {
                scale_y = scale;
            }
            for (int y1 = 0; y1 < scale_y; y1++) {
                for (int x1 = 0; x1 < scale_x; x1++) {
                    mui_canvas_draw_dot(p_canvas, x0 + x * scale + x1, y0 + y * scale + y1);
                }
            }
        }
    }
    mui_canvas_set_draw_color(p_canvas, color);
}

// Creates a single QR Code, then prints it to the console.
void draw_qrcode(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, uint8_t maxsize, const char *text) {
    enum qrcodegen_Ecc errCorLvl = qrcodegen_Ecc_LOW; // Error correction level

    // Make and print the QR Code symbol
    uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
    bool ok = qrcodegen_encodeText(text, tempBuffer, qrcode, errCorLvl, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX,
                                   qrcodegen_Mask_AUTO, true);
    if (ok) printQr(qrcode, p_canvas, x, y, maxsize);
}
