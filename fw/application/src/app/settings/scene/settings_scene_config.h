ADD_SCENE(settings, main, MAIN)
ADD_SCENE(settings, version, VERSION)
ADD_SCENE(settings, sleep_timeout, SLEEP_TIMEOUT)
ADD_SCENE(settings, lcd_backlight, LCD_BACKLIGHT)
#ifdef OLED_SCREEN
    ADD_SCENE(settings, oled_contrast, OLED_CONTRAST)
#endif