/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include "lvgl/lvgl.h"
//#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/display/drm.h"
#include "lvgl.h"
#include "lv_drivers/wayland/wayland.h"
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_conf.h"


lv_ui guider_ui;



bool close_cb(lv_disp_t * disp)
{
    printf("Inside cb\n");
    return 1;
}

void sig_handler(int signum)
{
    printf("\nInside Signal handler function\n");
    printf("------SIGINT signal catched------\n");
    printf("Program exit...\n");
    lv_deinit();
    #if USE_DRM && !USE_WAYLAND
        drm_exit();
        printf("DRM exit...\n");
    #endif
    #if LV_USE_GPU_NXP_G2D
        g2d_deinit();
        printf("G2D deinit...\n");
    #endif
    exit(0);
}

int main(void)
{
    signal(SIGINT, sig_handler);

    /*LVGL init*/
    lv_init();

#if LV_USE_GPU_NXP_G2D
    g2d_init();
#else
    img_init();
#endif

#if USE_WAYLAND      
    lv_wayland_init();
    lv_disp_t * disp = lv_wayland_create_window(HOR_RES,VER_RES,WAYLAND_SURF_TITLE,close_cb);

#elif USE_DRM
    /*DRM init*/
    #if LV_USE_GPU_NXP_G2D
        extern struct g2d_buf *ctx_buf;
        lv_color_t * buf = NULL;     
        drm_init();

        ctx_buf = g2d_alloc(HOR_RES * VER_RES * sizeof(lv_color_t), 1);
        buf = ctx_buf->buf_vaddr;

        // printf("drm_ctx_buf->buf_paddr:%x\n",ctx_buf->buf_paddr);
        // printf("drm_ctx_buf->buf_vaddr:%x\n",ctx_buf->buf_vaddr);
    #else
        static lv_color_t buf[DISP_BUF_SIZE]; 
        drm_init();
    #endif
    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    static lv_disp_t * disp;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = drm_flush;
    disp_drv.hor_res    = HOR_RES;
    disp_drv.ver_res    = VER_RES;
    disp=lv_disp_drv_register(&disp_drv);

    /* Linux input device init */
    evdev_init();
    
    /* Initialize and register a display input driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);      /*Basic initialization*/

    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = evdev_read;   //lv_gesture_dir_t lv_indev_get_gesture_dir(const lv_indev_t * indev)
    lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv); 
#else 
    printf("please set a display backend, Wayland or DRM.\n");
    return 0;
#endif    
    setup_ui(&guider_ui);
    events_init(&guider_ui);
    custom_init(&guider_ui);

    while(1) {
   
            lv_task_handler();
            //lv_tick_inc(5*1000);
            usleep(5000);

    }

    return 0;
}


/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
