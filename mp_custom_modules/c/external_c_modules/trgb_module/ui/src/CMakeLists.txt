SET(SOURCES screens/ui_Splash_Screen.c
    screens/ui_Welcome_Screen.c
    screens/ui_SettingScreen.c
    screens/ui_Loading_Screen.c
    screens/ui_Home_Screen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_logo_png.c)

add_library(ui ${SOURCES})
