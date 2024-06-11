// SPDX-License-Identifier: MIT
// Copyright 2020 NXP



#ifndef __CUSTOM_H_
#define __CUSTOM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"

void custom_init(lv_ui *ui);

#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */
#include "lv_conf.h"
extern uint8_t _PM5544_MK10_alpha_640x480_map[];
#if LV_USE_GPU_NXP_G2D
#include "g2d.h"
#include "kv.h"
void g2d_init();
#else
void img_init();
#endif
