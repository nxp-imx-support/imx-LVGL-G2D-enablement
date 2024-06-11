/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_g2d_blend.h"

#if LV_USE_GPU_NXP_G2D

/*********************
 *      DEFINES
 *********************/
#define G2D_TEMP_BUF_SIZE WAYLAND_HOR_RES * WAYLAND_VER_RES * 4U


extern struct g2d_buf *g2d_temp_buf;

extern struct g2d_buf *ctx_buf;
extern void *g2d_handle;
extern struct g2d_surface src_surf;
extern struct g2d_surface dst_surf;

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

// static LV_ATTRIBUTE_MEM_ALIGN uint8_t temp_buf[G2D_TEMP_BUF_SIZE];



/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_gpu_nxp_g2d_fill(const lv_area_t * dest_area, lv_coord_t dest_stride,
                         lv_color_t color, lv_opa_t opa)
{
    struct timeval start, end;
    double time_used;

    gettimeofday(&start, NULL);

    lv_coord_t dest_w = lv_area_get_width(dest_area);
    lv_coord_t dest_h = lv_area_get_height(dest_area);

    int src_buf_w = dest_w;
    int src_buf_h = dest_h;

    src_surf.left       = 0;
    src_surf.top        = 0;
    src_surf.right      = dest_w;
    src_surf.bottom     = dest_h;
    src_surf.stride     = dest_w;
    src_surf.width      = dest_w;
    src_surf.height     = dest_h;

    src_surf.planes[0]  = g2d_temp_buf->buf_paddr;
    src_surf.rot        = G2D_ROTATION_0;

    src_surf.clrcolor = (color.ch.alpha<<24)+(color.ch.blue<<16)+(color.ch.green<<8)+color.ch.red;

    g2d_clear(g2d_handle, &src_surf);
    g2d_flush(g2d_handle);

    dst_surf.left       = dest_area->x1;
    dst_surf.top        = dest_area->y1;
    dst_surf.right      = dest_area->x1+dest_w;
    dst_surf.bottom     = dest_area->y1+dest_h;
    dst_surf.stride     = dest_stride;
    dst_surf.width      = dest_w;
    dst_surf.height     = dest_h;

    dst_surf.planes[0]  = ctx_buf->buf_paddr;
    dst_surf.rot        = G2D_ROTATION_0;

    src_surf.global_alpha = opa;
    dst_surf.global_alpha = 0xff;
 
    src_surf.blendfunc = G2D_ONE | G2D_PRE_MULTIPLIED_ALPHA;
    dst_surf.blendfunc = G2D_ONE_MINUS_SRC_ALPHA | G2D_PRE_MULTIPLIED_ALPHA;

    g2d_enable(g2d_handle, G2D_GLOBAL_ALPHA);
    g2d_enable(g2d_handle,G2D_BLEND);
    g2d_blit(g2d_handle, &src_surf, &dst_surf); // launch G2D processing
    g2d_disable(g2d_handle,G2D_BLEND);
    g2d_disable(g2d_handle, G2D_GLOBAL_ALPHA);
    // g2d_finish(g2d_handle);

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec); 
    printf("lv_gpu_nxp_g2d_fill Time used: %f us\n", time_used);
}


void lv_gpu_nxp_g2d_blit(const lv_area_t * dest_area, lv_coord_t dest_stride, int src_buf_paddr, 
                        const lv_area_t * src_area, lv_coord_t src_stride, lv_opa_t opa)
{
    struct timeval start, end;
    double time_used;

    gettimeofday(&start, NULL);

    lv_coord_t dest_w = lv_area_get_width(dest_area);
    lv_coord_t dest_h = lv_area_get_height(dest_area);
    lv_coord_t src_w = lv_area_get_width(src_area);
    lv_coord_t src_h = lv_area_get_height(src_area);

    src_surf.left       = src_area->x1;
    src_surf.top        = src_area->y1;
    src_surf.right      = src_area->x1+src_w;
    src_surf.bottom     = src_area->y1+src_h;
    src_surf.stride     = src_stride;
    src_surf.width      = src_w;
    src_surf.height     = src_h;

    src_surf.planes[0]  = src_buf_paddr;
    src_surf.rot        = G2D_ROTATION_0;

    dst_surf.left       = dest_area->x1;
    dst_surf.top        = dest_area->y1;
    dst_surf.right      = dest_area->x1+dest_w;
    dst_surf.bottom     = dest_area->y1+dest_h;
    dst_surf.stride     = dest_stride;
    dst_surf.width      = dest_w;
    dst_surf.height     = dest_h;

    dst_surf.planes[0]  = ctx_buf->buf_paddr;
    dst_surf.rot        = G2D_ROTATION_0;

    src_surf.global_alpha = opa;
    dst_surf.global_alpha = 0xff;

    g2d_cache_op(ctx_buf, G2D_CACHE_FLUSH);

    src_surf.blendfunc = G2D_ONE | G2D_PRE_MULTIPLIED_ALPHA;
    dst_surf.blendfunc = G2D_ONE_MINUS_SRC_ALPHA | G2D_PRE_MULTIPLIED_ALPHA;

    g2d_enable(g2d_handle, G2D_GLOBAL_ALPHA);
    g2d_enable(g2d_handle,G2D_BLEND);
    g2d_blit(g2d_handle, &src_surf, &dst_surf); // launch G2D processing
    g2d_disable(g2d_handle,G2D_BLEND);
    g2d_disable(g2d_handle, G2D_GLOBAL_ALPHA);
    // g2d_finish(g2d_handle);

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec); 
    printf("lv_gpu_nxp_g2d_blit Time used: %f us\n", time_used);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

void lv_gpu_nxp_g2d_blit_rot(const lv_area_t * dest_area, lv_coord_t dest_stride,
                            const lv_area_t * src_area, lv_coord_t src_stride, int src_buf_paddr, 
                            enum g2d_rotation g2d_angle, lv_opa_t opa)
{
    struct timeval start, end;
    double time_used;

    gettimeofday(&start, NULL);
 
    lv_coord_t dest_w = lv_area_get_width(dest_area);
    lv_coord_t dest_h = lv_area_get_height(dest_area);
    lv_coord_t src_w = lv_area_get_width(src_area);
    lv_coord_t src_h = lv_area_get_height(src_area);

    src_surf.left       = src_area->x1;
    src_surf.top        = src_area->y1;
    src_surf.right      = src_area->x1+src_w;
    src_surf.bottom     = src_area->y1+src_h;
    src_surf.stride     = src_stride;
    src_surf.width      = src_w;
    src_surf.height     = src_h;

    src_surf.planes[0]  = src_buf_paddr;
    src_surf.rot        = g2d_angle;

    dst_surf.left       = dest_area->x1;
    dst_surf.top        = dest_area->y1;
    dst_surf.right      = dest_area->x1+dest_w;
    dst_surf.bottom     = dest_area->y1+dest_h;
    dst_surf.stride     = dest_stride;
    dst_surf.width      = dest_w;
    dst_surf.height     = dest_h;

    dst_surf.planes[0]  = ctx_buf->buf_paddr;
    dst_surf.rot        = G2D_ROTATION_0;

    src_surf.global_alpha = opa;
    dst_surf.global_alpha = 0xff;

    g2d_cache_op(ctx_buf, G2D_CACHE_FLUSH);

    src_surf.blendfunc = G2D_ONE | G2D_PRE_MULTIPLIED_ALPHA;
    dst_surf.blendfunc = G2D_ONE_MINUS_SRC_ALPHA | G2D_PRE_MULTIPLIED_ALPHA;

    g2d_enable(g2d_handle, G2D_GLOBAL_ALPHA);
    g2d_enable(g2d_handle,G2D_BLEND);
    g2d_blit(g2d_handle, &src_surf, &dst_surf); // launch G2D processing
    g2d_disable(g2d_handle,G2D_BLEND);
    g2d_disable(g2d_handle, G2D_GLOBAL_ALPHA);
    // g2d_finish(g2d_handle);

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("lv_gpu_nxp_g2d_blit_rot Time used: %f us\n", time_used);
}



void lv_gpu_nxp_g2d_wait(){
    g2d_finish(g2d_handle);
}


#endif /*LV_USE_GPU_NXP_G2D*/
