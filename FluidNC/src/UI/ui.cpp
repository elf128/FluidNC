#if defined( INCLUDE_LVGL )
// This code doesn't have to be compiled if LCD is not defined in the project.

#include "ui.h"
//#include "ui_helpers.h"
#include "src/Logging.h"
#include "TFT_eSPI.h"
//#include "extra/lv_extra.h"


TaskHandle_t        lv_disp_tcb = NULL;
TaskHandle_t        frame_task_tcb = NULL;
//SemaphoreHandle_t   is_frame_need = NULL;


static char testStr[] = "Hello";
void ui_test()
{
// Create simple label 
    lv_obj_t *label = lv_label_create( lv_scr_act() );
    lv_label_set_text( label, testStr );
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
}

void UI::ui_Screen1_screen_init()
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen1_Panel1 = lv_obj_create(ui_Screen1);
    lv_obj_set_height(ui_Screen1_Panel1, 113);
    lv_obj_set_width(ui_Screen1_Panel1, lv_pct(69));
    lv_obj_set_y(ui_Screen1_Panel1, -37);
    lv_obj_set_x(ui_Screen1_Panel1, lv_pct(-3));
    lv_obj_set_align(ui_Screen1_Panel1, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Screen1_Panel1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

}

void UI::ui_init()
{
    lv_theme_t * theme = lv_theme_default_init(
                                display, 
                                lv_palette_main( LV_PALETTE_BLUE ), 
                                lv_palette_main( LV_PALETTE_RED ),
                                true, LV_FONT_DEFAULT);

    lv_disp_set_theme( display, theme);
    //ui_test();
    ui_Screen1_screen_init();
    lv_disp_load_scr(ui_Screen1);
}

void UI::ui_frame()
{

}

void UI::ui_update()
{

}

void UI::tft_init()
{
    tft_interface   = new TFT_eSPI();
    disp_buf        = new lv_disp_draw_buf_t();
    update_storage  = new lv_color_t[ c_LV_BUF_SIZE ];
    disp_drv        = new lv_disp_drv_t ();
    input_drv       = new lv_indev_drv_t ();

    tft_interface->init();
    tft_interface->setRotation( TFT_ROTATE );
    tft_interface->fillScreen( TFT_BLUE ); // Initial flood with blue color. If lvgl does nothing, that's what you'll see on the screen 
    tft_interface->fillRectVGradient( 0, 0, 160, 160, TFT_MAGENTA, TFT_YELLOW );

    lv_init();
#if LV_USE_LOG != 0
    // register print function for debugging 
    lv_log_register_print_cb( [](const char * buf)
    {
        log_info(buf);
    } ); 
#endif

    lv_disp_draw_buf_init( disp_buf, update_storage, NULL, c_LV_BUF_SIZE );  

    /* display driver register */
    lv_disp_drv_init( disp_drv );

    disp_drv->hor_res = TFT_WIDTH;
    disp_drv->ver_res = TFT_HEIGHT;
    disp_drv->flush_cb = [](lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p ) 
    {
        uint32_t w = (area->x2 - area->x1 + 1);
        uint32_t h = (area->y2 - area->y1 + 1);
        tft_interface->startWrite();
        tft_interface->setAddrWindow( area->x1, area->y1, w, h );
        tft_interface->pushPixels( &color_p->full, w * h );
        //tft_interface->pushPixelsDMA(&color_p->full, w * h );
        tft_interface->endWrite();
        
        lv_disp_flush_ready( drv );
    };

    disp_drv->draw_buf = disp_buf;
    lv_disp_drv_register( disp_drv );

    lv_indev_drv_init( input_drv );

    input_drv->type = LV_INDEV_TYPE_POINTER;
    input_drv->read_cb = [](_lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
    {
        uint16_t touchX=0, touchY=0;
        static uint16_t last_x = 0;
        static uint16_t last_y = 0;

        bool touched = tft_interface->getTouch(&touchX, &touchY);

        touchX = (touchX > TFT_WIDTH) ? TFT_WIDTH : touchX;
        touchY = (touchY > TFT_HEIGHT) ? TFT_HEIGHT : touchY;
        
        if( touched ) 
        {
            data->point.x = last_x = touchX;
            data->point.y = last_y = touchY;
            data->state = LV_INDEV_STATE_PR;
            BEEP_ON;
        }
        else 
        {
            data->point.x = last_x;
            data->point.y = last_y;
            data->state = LV_INDEV_STATE_REL;
            BEEP_OFF;
        }
    };

    lv_indev_drv_register( input_drv );
}

IRAM_ATTR void lvgl_disp_task(void *parg)
{
    log_info( "Display task started" );

    pinMode( TFT_EN, OUTPUT);
    digitalWrite( TFT_EN, HIGH );

    TickType_t       xLastWakeTime;
    const TickType_t xDisplayFrequency = 5;                  
    xLastWakeTime = xTaskGetTickCount();   

    bool logo = true;   
    uint16_t logo_count = 0; 

    //mks_global_style_init();
    //mks_draw_logo();

    //is_frame_need = xSemaphoreCreateBinary();
    //frame_task_init();
    //mks_grbl.wifi_connect_enable = true;

    log_info( "Display task loop started" );
    
    while(1) 
    {
        lv_task_handler();
        if(logo == true) 
        {
            logo_count++;

            if( logo_count == 100 )
                digitalWrite( TFT_EN, LOW );

            if( logo_count == 400 )
            {
                //ui_lv_clean_ui();
                //ui_draw_ready();
                /*

                if(mks_updata.updata_flag == UD_HAD_FILE) 
                {
                    mks_draw_updata();
                }else {
                    if(mks_grbl.is_test_mode == true) 
                    {
                        mks_draw_test_ui();
                    }else{
                        mks_draw_ready();
                    }
                }*/
                logo = false;
            }
        }else
            UI::ui_update();

        //vTaskDelayUntil(&xLastWakeTime, xDisplayFrequency);
        vTaskDelay(20); 
    }
}

//-------------------------------------------------------------------------------------------------
/*
IRAM_ATTR void frame_task(void *parg) 
{
    log_info( "Creat frame task succeed" );

    BaseType_t sem_receive = pdPASS;

    while(1) 
    {  
        sem_receive = xSemaphoreTake(is_fram_need, portMAX_DELAY);

        if(sem_receive == pdTRUE)
        {

            grbl_send(CLIENT_SERIAL,"receive sem succeed\n");
            vTaskSuspend(lv_disp_tcb);
            mks_run_frame(frame_ctrl.file_name);
            vTaskResume(lv_disp_tcb); 
        }
    }
}

IRAM_ATTR void frame_task_init(void ) 
{

    xTaskCreatePinnedToCore(frame_task,             // task
                            "Frame task",           // name for task
                            FRAME_TASK_STACK,       // size of task stack
                            NULL,                   // parameters
                            FRAME_TASK_PRO,         // priority
                            // nullptr,
                            &frame_task_tcb,
                            FRAME_TASK_CORE         // must run the task on same core
                                                    // core
    );
}

*/

void display_init()
 {
    log_info( "Display init" );

    UI::tft_init();
    UI::ui_init();
    
    /*
    xTaskCreatePinnedToCore(lvgl_disp_task,     // task
                            "lvglTask",         // name for task
                            UI::c_taskStack,    // size of task stack
                            NULL,               // parameters
                            UI::c_taskPrio,     // priority
                            // nullptr,
                            &lv_disp_tcb,
                            UI::c_taskCore      // core
    );*/

}

TFT_eSPI*           UI::tft_interface  = nullptr;
lv_disp_t*          UI::display        = nullptr;
lv_disp_draw_buf_t* UI::disp_buf       = nullptr;
lv_color_t*         UI::update_storage = nullptr;
lv_disp_drv_t*      UI::disp_drv       = nullptr;
lv_indev_drv_t*     UI::input_drv      = nullptr;

lv_obj_t* UI::ui_Screen1;
lv_obj_t* UI::ui_Screen1_Panel1;

#endif
