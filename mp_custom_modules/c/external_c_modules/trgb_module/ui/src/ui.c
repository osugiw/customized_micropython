// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.2
// LVGL version: 8.3.6
// Project name: TRGB_ESP32S3

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
void RecordAnim_Animation( lv_obj_t *TargetObject, int delay);


// SCREEN: ui_Splash_Screen
void ui_Splash_Screen_screen_init(void);
void ui_event_Splash_Screen( lv_event_t * e);
lv_obj_t *ui_Splash_Screen;
void ui_event_Image1( lv_event_t * e);
lv_obj_t *ui_Image1;


// SCREEN: ui_Welcome_Screen
void ui_Welcome_Screen_screen_init(void);
lv_obj_t *ui_Welcome_Screen;
lv_obj_t *ui_Image2;
void ui_event_NextBT( lv_event_t * e);
lv_obj_t *ui_NextBT;
lv_obj_t *ui_Label4;
lv_obj_t *ui_Description;


// SCREEN: ui_SettingScreen
void ui_SettingScreen_screen_init(void);
lv_obj_t *ui_SettingScreen;
lv_obj_t *ui_Panel1;
lv_obj_t *ui_Label1;
lv_obj_t *ui_SensitivityValue;
void ui_event_SensitivitySlider( lv_event_t * e);
lv_obj_t *ui_SensitivitySlider;
lv_obj_t *ui_SensitivityTxt;
lv_obj_t *ui_MicrophoneSettings;
lv_obj_t *ui_GainTxt;
void ui_event_GainSlider( lv_event_t * e);
lv_obj_t *ui_GainSlider;
lv_obj_t *ui_GainValue;
lv_obj_t *ui_NoiseCancelling;
void ui_event_NextBT1( lv_event_t * e);
lv_obj_t *ui_NextBT1;
lv_obj_t *ui_Label2;
void ui_event_SwitchNoise( lv_event_t * e);
lv_obj_t *ui_SwitchNoise;


// SCREEN: ui_Loading_Screen
void ui_Loading_Screen_screen_init(void);
void ui_event_Loading_Screen( lv_event_t * e);
lv_obj_t *ui_Loading_Screen;
void ui_event_Spinner1( lv_event_t * e);
lv_obj_t *ui_Spinner1;
lv_obj_t *ui_Label7;


// SCREEN: ui_Home_Screen
void ui_Home_Screen_screen_init(void);
lv_obj_t *ui_Home_Screen;
lv_obj_t *ui_Image3;
lv_obj_t *ui_Label3;
lv_obj_t *ui_Label5;
void ui_event_RecordBT( lv_event_t * e);
lv_obj_t *ui_RecordBT;
lv_obj_t *ui_Label6;
lv_obj_t *ui____initial_actions0;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////
void RecordAnim_Animation( lv_obj_t *TargetObject, int delay)
{
ui_anim_user_data_t *PropertyAnimation_0_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
PropertyAnimation_0_user_data->target = TargetObject;
PropertyAnimation_0_user_data->val = -1;
lv_anim_t PropertyAnimation_0;
lv_anim_init(&PropertyAnimation_0);
lv_anim_set_time(&PropertyAnimation_0, 5000);
lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity );
lv_anim_set_values(&PropertyAnimation_0, 100, 255 );
lv_anim_set_path_cb( &PropertyAnimation_0, lv_anim_path_ease_out);
lv_anim_set_delay( &PropertyAnimation_0, delay + 0 );
lv_anim_set_deleted_cb( &PropertyAnimation_0, _ui_anim_callback_free_user_data );
lv_anim_set_playback_time(&PropertyAnimation_0, 0);
lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
 lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
lv_anim_set_early_apply( &PropertyAnimation_0, false );
 lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity );
lv_anim_start(&PropertyAnimation_0);

}

///////////////////// FUNCTIONS ////////////////////
void ui_event_Splash_Screen( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_SCREEN_LOADED) {
      _ui_screen_change( &ui_Welcome_Screen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 2000, &ui_Welcome_Screen_screen_init);
}
}
void ui_event_Image1( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_SCREEN_LOADED) {
      _ui_screen_change( &ui_Welcome_Screen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, &ui_Welcome_Screen_screen_init);
}
}
void ui_event_NextBT( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      _ui_screen_change( &ui_SettingScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, &ui_SettingScreen_screen_init);
}
}
void ui_event_SensitivitySlider( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_VALUE_CHANGED) {
        _ui_slider_set_text_value( ui_GainValue, target, "", "%");
        char buff[8];
        lv_snprintf(buff, sizeof(buff), "%d%%", (int)lv_slider_get_value(target));
        lv_label_set_text(ui_SensitivityValue, buff);
    }
}
void ui_event_GainSlider( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_VALUE_CHANGED) {
        _ui_slider_set_text_value( ui_GainValue, target, "", "%");
        char buff[8];
        lv_snprintf(buff, sizeof(buff), "%d%%", (int)lv_slider_get_value(target));
        lv_label_set_text(ui_GainValue, buff);
    }
}
void ui_event_NextBT1( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_CLICKED) {
          _ui_screen_change( &ui_Loading_Screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, &ui_Loading_Screen_screen_init);
          microphoneSetting_func( e );
    }
}
void ui_event_SwitchNoise( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_VALUE_CHANGED) {
        noiseCancelling_func( e );
        LV_LOG_USER("State: %s\n", lv_obj_has_state(target, LV_STATE_CHECKED) ? "Noise Cancelling On" : "Noise Cancelling Off");
    }
}
void ui_event_Loading_Screen( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_SCREEN_LOADED) {
        _ui_screen_change( &ui_Home_Screen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 1000, &ui_Home_Screen_screen_init);
    }
}
void ui_event_Spinner1( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_SCREEN_LOADED) {
        _ui_screen_change( &ui_Home_Screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 2000, &ui_Home_Screen_screen_init);
    }
}
void ui_event_RecordBT( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      RecordAnim_Animation(ui_RecordBT, 200);
}
}

///////////////////// SCREENS ////////////////////

void ui_init( void )
{
lv_disp_t *dispp = lv_disp_get_default();
lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
lv_disp_set_theme(dispp, theme);
ui_Splash_Screen_screen_init();
ui_Welcome_Screen_screen_init();
ui_SettingScreen_screen_init();
ui_Loading_Screen_screen_init();
ui_Home_Screen_screen_init();
ui____initial_actions0 = lv_obj_create(NULL);
lv_disp_load_scr( ui_Splash_Screen);
}