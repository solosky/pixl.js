ADD_SCENE(settings, main, MAIN)
ADD_SCENE(settings, version, VERSION)
ADD_SCENE(settings, sleep_timeout, SLEEP_TIMEOUT)
#ifdef LCD_SCREEN
ADD_SCENE(settings, lcd_backlight, LCD_BACKLIGHT)
#endif
#ifdef OLED_SCREEN
    ADD_SCENE(settings, oled_contrast, OLED_CONTRAST)
#endif
ADD_SCENE(settings, language, LANGUAGE)
ADD_SCENE(settings, storage, STORAGE)