#ifndef __UI_H__
#define __UI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

//extern lv_obj_t * ui_Screen1;
//extern lv_obj_t * ui_Screen1_Panel1;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#include <stdint.h>
#include <sys/types.h>

class TFT_eSPI;

class UI
{
    static TFT_eSPI*           tft_interface;
    static lv_disp_t*          display;
    static lv_disp_draw_buf_t* disp_buf;
    static lv_color_t*         update_storage;
    static lv_disp_drv_t*      disp_drv;
    static lv_indev_drv_t*     input_drv;

    static const uint c_LV_BUF_SIZE = 10240;

    static lv_obj_t* ui_Screen1;
    static lv_obj_t* ui_Screen1_Panel1;

    static void ui_Screen1_screen_init();
public:
    static const uint c_taskStack   = 4096*2;
    static const uint c_taskPrio    = 2;
    static const uint c_taskCore    = 1;

    UI() {};
    ~UI(){};

    static void tft_init();
    static void ui_init();
    static void ui_update();
    static void ui_frame();
};

//void ui_init();
//void ui_update();
//void ui_frame();

#if defined(USE_BEEP)
#define BEEP_ON             digitalWrite(TFT_BEEPER, HIGH)
#define BEEP_OFF            digitalWrite(TFT_BEEPER, LOW)
#else 
#define BEEP_ON            
#define BEEP_OFF           
#endif

#endif
