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
#include "../../sw/lv_draw_sw.h"
#include <stdlib.h>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
extern struct g2d_buf *g2d_temp_buf;

extern struct g2d_buf *ctx_buf;
extern void *g2d_handle;
extern struct g2d_surface src_surf;
extern struct g2d_surface dst_surf;
/**
 * Fill area, with optional opacity.
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dest_stride Stride of destination buffer in pixels
 * @param[in] color Color
 * @param[in] opa Opacity
 */
void lv_gpu_nxp_g2d_fill(const lv_area_t * dest_area, lv_coord_t dest_stride,
                         lv_color_t color, lv_opa_t opa);

/**
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with effects.
 * With optional opacity.
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dest_stride Stride of destination buffer in pixels
 * @param[in] src_buf_paddr physical address of src buf used for g2d pipleine
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] src_stride Stride of source buffer in pixels
 * @param[in] opa Opacity
 */
void lv_gpu_nxp_g2d_blit(const lv_area_t * dest_area, lv_coord_t dest_stride,
                         int src_buf_paddr, const lv_area_t * src_area, lv_coord_t src_stride,
                         lv_opa_t opa);


/**
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with effects.
 * With optional opacity and rotation.
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dest_stride Stride of destination buffer in pixels
 * @param[in] src_buf_paddr physical address of src buf used for g2d pipleine
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] src_stride Stride of source buffer in pixels
 * @param[in] g2d_angle angle
 * @param[in] opa Opacity
 */



void lv_gpu_nxp_g2d_blit_rot(const lv_area_t * dest_area, lv_coord_t dest_stride,
                            const lv_area_t * src_area, lv_coord_t src_stride, int src_buf_paddr, 
                            enum g2d_rotation g2d_angle, lv_opa_t opa);

void lv_gpu_nxp_g2d_blit_cover(int temp_buf_paddr, lv_area_t * rel_coords, lv_draw_img_dsc_t * dsc);

void lv_gpu_nxp_g2d_wait();

// void lv_gpu_nxp_g2d_buffer_copy(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
//                                 const lv_color_t * src_buf, const lv_area_t * src_area, lv_coord_t src_stride);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_NXP_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

