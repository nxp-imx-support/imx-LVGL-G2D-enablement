/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"


static void screen_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_SCREEN_LOADED:
	{
		//Write animation: screen_img_1 move in x direction
		lv_anim_t screen_img_1_anim_x;
		lv_anim_init(&screen_img_1_anim_x);
		lv_anim_set_var(&screen_img_1_anim_x, guider_ui.screen_img_1);
		lv_anim_set_time(&screen_img_1_anim_x, 1500);
		lv_anim_set_delay(&screen_img_1_anim_x, 0);
		lv_anim_set_exec_cb(&screen_img_1_anim_x, (lv_anim_exec_xcb_t)lv_obj_set_x);
		lv_anim_set_values(&screen_img_1_anim_x, lv_obj_get_x(guider_ui.screen_img_1), 600);
		lv_anim_set_path_cb(&screen_img_1_anim_x, &lv_anim_path_linear);
		screen_img_1_anim_x.repeat_cnt = LV_ANIM_REPEAT_INFINITE;
		lv_anim_start(&screen_img_1_anim_x);
		//Write animation: screen_img_1 move in y direction
		lv_anim_t screen_img_1_anim_y;
		lv_anim_init(&screen_img_1_anim_y);
		lv_anim_set_var(&screen_img_1_anim_y, guider_ui.screen_img_1);
		lv_anim_set_time(&screen_img_1_anim_y, 1500);
		lv_anim_set_delay(&screen_img_1_anim_y, 0);
		lv_anim_set_exec_cb(&screen_img_1_anim_y, (lv_anim_exec_xcb_t)lv_obj_set_y);
		lv_anim_set_values(&screen_img_1_anim_y, lv_obj_get_y(guider_ui.screen_img_1), 0);
		lv_anim_set_path_cb(&screen_img_1_anim_y, &lv_anim_path_linear);
		screen_img_1_anim_y.repeat_cnt = LV_ANIM_REPEAT_INFINITE;
		lv_anim_start(&screen_img_1_anim_y);
		break;
	}
	default:
		break;
	}
}
void events_init_screen(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->screen, screen_event_handler, LV_EVENT_ALL, NULL);
}

void events_init(lv_ui *ui)
{

}
