#include "lcd_drv.h"
#include "bitmap.h"
#include "lcd_comm.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "nrf_log.h"
#include <stdio.h>

#define LCD_X_SIZE 128
#define LCD_Y_SIZE 64

static bool m_screen_enabled = true;
static uint8_t m_display_content[LCD_Y_SIZE / 8][LCD_X_SIZE];

void delay(unsigned int delay) //�ӳٳ���
{
    unsigned int i, j;

    for (i = 0; i < delay; i++)
        for (j = 0; j < 100; j++)
            ;
}

void lcd_init(void) {
    lcd_comm_init();
    lcd_reset(); //Ӳ��λ

    lcd_write_command(0xe2); //����λ
    delay(10);
    lcd_write_command(0xa2); //��ѹ��·,��ѹ������·
    lcd_write_command(0xa0); // 0x20~0x27 ΪV5��ѹ�ڲ�����������ￄ1�7
    lcd_write_command(0xc8); //����EV �Աȶ�
    lcd_write_command(0x23); //�Աȶ�ֵ

    lcd_write_command(0x81); // 0x01~0x3f�����Ĵ�������ģʽ

    lcd_write_command(0x32); // 0xa0ΪSegment����, 0xa1ΪSegment����
    lcd_write_command(0x2f); // 0xc0����ɨ��, 0xc8����ɨ��
    lcd_write_command(0xb0); // 0xa6������ʾ, 0xa7������ʾ
    lcd_write_command(0xAF); // 0xa4������ʾ, 0xa5ȫ����ʾ
    lcd_write_command(0xA6); //��ѹ������
                             // lcd_write_command(0x10);		//��ѹ��ֵ:0~10
                             // lcd_write_command(0xaf);		//����ʾ
}

void lcd_uninit() { lcd_comm_uninit(); }

static void lcd_set_pos(uint8_t page, uint8_t column) {
    lcd_write_command(0xb0 + (0x07 & page));
    lcd_write_command(0x10 | column >> 4);
    lcd_write_command(column & 0x0f);
}

static void display(uint8_t page_start, uint8_t page_num, uint8_t column, uint8_t width, const uint8_t *data) {
    uint32_t valueIndex = 0;
    uint8_t page_count = page_start;

    for (uint8_t idx = 0; idx < page_num; idx++, page_count++, valueIndex += width) {
        lcd_set_pos(page_count, column);
        lcd_write_data((uint8_t *)(data + valueIndex), width);
    }
}

void lcd_show_pic(uint8_t col, uint8_t row, uint8_t width, uint8_t high, const uint8_t *data) {
    if (!m_screen_enabled) return;

    if (row + high <= 0 || row >= LCD_Y_SIZE || high > LCD_Y_SIZE || col + width <= 0 || col >= LCD_X_SIZE ||
        width > LCD_X_SIZE)
        return;

    int8_t start_row = (row % 8);        //ҳ����ʼ��
    uint8_t stop_row = (row + high) % 8; //ҳ�ڽ�����
    uint8_t page_offset = 0;             //ƫ��ҳ
    uint8_t start_page = 0;              //��ʼҳ
    uint8_t real_page = 0;               //ʵ��ҳ��
    uint8_t str_page = (high + 7) / 8;   //�ַ�ҳ��
    uint8_t col_offset = 0;              //ƫ����
    uint8_t real_width = width;          //ʵ������

    if (row < 0) {
        if (start_row) start_row += 8;
        real_page = (row + high + 7) / 8;
        page_offset = (-row + 7) / 8;
    } else {
        start_page = (row / 8);
        if (row + high > LCD_Y_SIZE) {
            stop_row = 0;
            real_page = (LCD_Y_SIZE - row + 7) / 8;
        } else
            real_page = (start_row + high + 7) / 8;
    }

    if (col < 0) {
        col_offset = -col;
        real_width += col;
        col = 0;
    }
    uint8_t *displayVal = (uint8_t *)malloc(real_page * real_width);
    memset(displayVal, 0, real_page * real_width);

    if (data == NULL) data = displayVal;

    for (uint8_t idx = 0; idx < real_width; idx++) {
        for (uint8_t num = 0, median = 0; num < real_page; num++) {
            uint8_t front_half = 0; //ǰ�벿��
            uint8_t lower_half = 0; //��벿�ￄ1�7
            uint16_t real_index = num * real_width + idx;
            uint16_t str_index = (page_offset + num) * width + col_offset + idx;

            if (page_offset + num < str_page) {
                front_half = data[str_index] << start_row;       //ǰ�벿��
                lower_half = data[str_index] >> (8 - start_row); //��벿�ￄ1�7
            }

            if (num == 0) {
                if (row >= 0)
                    displayVal[real_index] =
                        front_half | (m_display_content[start_page][col + idx] & (0xFF >> (8 - start_row)));
                else
                    displayVal[real_index] =
                        front_half | (data[(page_offset - 1) * width + col_offset + idx] >> (8 - start_row));
            } else
                displayVal[real_index] = front_half | median;

            if (num == (real_page - 1) && stop_row) {
                displayVal[real_index] &= ~(0xFF << stop_row);
                displayVal[real_index] |= (m_display_content[start_page + num][col + idx] & (0xFF << stop_row));
            }
            m_display_content[start_page + num][col + idx] = displayVal[real_index];
            median = lower_half;
        }
    }
    display(start_page, real_page, col, real_width, displayVal);

    free(displayVal);
    displayVal = NULL;
}

void lcd_clear(void) {
    if (!m_screen_enabled) return;

    memset(m_display_content, 0x00, sizeof(m_display_content));
    lcd_show_pic(0, 0, LCD_X_SIZE, LCD_Y_SIZE, (uint8_t *)m_display_content);
}

void show_char_0608(uint8_t x, uint8_t y, uint8_t chr) {
    chr -= ' ';
    lcd_show_pic(x, y, 6, 8, figure_6x8_str[chr]);
}

void lcd_show_char(uint8_t x, uint8_t y, uint8_t chr) {
    chr -= ' ';
    lcd_show_pic(x, y, 7, 16, ascii_1608[chr]);
}

void show_str_0608(uint8_t x, uint8_t y, uint8_t *data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        show_char_0608(x + i * 6, y, *(data + i));
    }
}

void show_str_1608(uint8_t x, uint8_t y, uint8_t *data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        lcd_show_char(x + i * 8, y, *(data + i));
    }
}

void lcd_show_font(uint8_t x, uint8_t y, uint8_t t_x, uint8_t t_y, uint8_t *font) {
    for (uint8_t i = 0; i < ((uint8_t)(t_y + 7) / 8); i++) {
        lcd_show_pic(x, y + i * 8, t_x, 8, &font[i * t_x]);
    }
}


void lcd_draw_str_0608(uint8_t x, uint8_t y, char *str) { show_str_0608(x, y, (uint8_t *)str, strlen(str)); }

void lcd_draw_str_1608(uint8_t x, uint8_t y, char *str) { show_str_1608(x, y, (uint8_t *)str, strlen(str)); }
