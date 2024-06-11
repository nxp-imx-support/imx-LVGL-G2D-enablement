/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#include "lv_conf.h"
#if LV_USE_GPU_NXP_G2D
#include "custom.h"
#include "lv_draw_g2d_blend.h"
#include "../../sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_draw_sw_ctx_t lv_draw_g2d_ctx_t;

extern struct g2d_buf *g2d_temp_buf;
extern struct g2d_buf *ctx_buf;
extern void *g2d_handle;
extern struct g2d_surface src_surf;
extern struct g2d_surface dst_surf;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_g2d_ctx_init(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);
void lv_draw_g2d_ctx_deinit(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_GPU_NXP_G2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

