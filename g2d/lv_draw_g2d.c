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

#include <unistd.h>
#include "lv_draw_g2d.h"
#if LV_USE_GPU_NXP_G2D
#include "lv_draw_g2d_blend.h"
#include "../../../hal/lv_hal_tick.h"
#if LV_COLOR_DEPTH != 32
    #include "../../../core/lv_refr.h"
#endif

/*********************
 *      DEFINES
 *********************/

/* Minimum area (in pixels) for PXP blit/fill processing. */
#ifndef LV_GPU_NXP_G2D_SIZE_LIMIT
    #define LV_GPU_NXP_G2D_SIZE_LIMIT 256
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_draw_g2d_wait_for_finish(lv_draw_ctx_t * draw_ctx);

static void lv_draw_g2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

static void lv_draw_g2d_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                    const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t cf);

static void lv_draw_g2d_buffer_copy(lv_draw_ctx_t * draw_ctx,
                                    void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                                    void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_g2d_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_g2d_ctx_t * g2d_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;
    g2d_draw_ctx->base_draw.draw_img_decoded = lv_draw_g2d_img_decoded;
    g2d_draw_ctx->blend = lv_draw_g2d_blend;
    g2d_draw_ctx->base_draw.wait_for_finish = lv_draw_g2d_wait_for_finish;
    g2d_draw_ctx->base_draw.buffer_copy = lv_draw_sw_buffer_copy;
}

void lv_draw_g2d_ctx_deinit(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    lv_draw_sw_deinit_ctx(drv, draw_ctx);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * During rendering, LVGL might initializes new draw_ctxs and start drawing into
 * a separate buffer (called layer). If the content to be rendered has "holes",
 * e.g. rounded corner, LVGL temporarily sets the disp_drv.screen_transp flag.
 * It means the renderers should draw into an ARGB buffer.
 * With 32 bit color depth it's not a big problem but with 16 bit color depth
 * the target pixel format is ARGB8565 which is not supported by the GPU.
 * In this case, the PXP callbacks should fallback to SW rendering.
 */
static inline bool need_argb8565_support()
{
#if LV_COLOR_DEPTH != 32
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    if(disp->driver->screen_transp == 1)
        return true;
#endif

    return false;
}

static void lv_draw_g2d_wait_for_finish(lv_draw_ctx_t * draw_ctx)
{
    lv_gpu_nxp_g2d_wait();

    lv_draw_sw_wait_for_finish(draw_ctx);
}

static void lv_draw_g2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;
#if LV_NXP_G2D_IMG_ONLY
    lv_draw_sw_blend_basic(draw_ctx, dsc); return;
#endif
    
    if(need_argb8565_support()) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        return;
    }


    lv_area_t blend_area;
    /*Let's get the blend area which is the intersection of the area to draw and the clip area*/
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area))
        return; /*Fully clipped, nothing to do*/

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
    if(dsc->mask_buf != NULL || dsc->blend_mode != LV_BLEND_MODE_NORMAL ||
       lv_area_get_size(&blend_area) < LV_GPU_NXP_G2D_SIZE_LIMIT) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        return;
    }

    /*Fill/Blend only non masked, normal blended*/
    lv_color_t * dest_buf = draw_ctx->buf;
    lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);
    const lv_color_t * src_buf = dsc->src_buf;
    if(src_buf == NULL) {
         lv_gpu_nxp_g2d_fill(&blend_area, dest_stride, dsc->color, dsc->opa);
    }
    else {
        lv_area_t src_area;
        src_area.x1 = blend_area.x1 - (dsc->blend_area->x1 - draw_ctx->buf_area->x1);
        src_area.y1 = blend_area.y1 - (dsc->blend_area->y1 - draw_ctx->buf_area->y1);
        src_area.x2 = src_area.x1 + lv_area_get_width(&blend_area) - 1;
        src_area.y2 = src_area.y1 + lv_area_get_height(&blend_area) - 1;
        lv_coord_t src_stride = lv_area_get_width(&blend_area);

        char kv_key[20];
        sprintf(kv_key, "%x", src_buf); 
        int src_buf_paddr = get(kv_key);
        lv_gpu_nxp_g2d_blit(&blend_area, dest_stride, src_buf_paddr, &src_area, src_stride, dsc->opa);

    }
}

static void lv_draw_g2d_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                    const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t cf)
{
    if(need_argb8565_support()) {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }

    const lv_color_t * src_buf = (const lv_color_t *)map_p;
    if(!src_buf) {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }
    lv_color_t *temp_buf = g2d_temp_buf->buf_vaddr;
    int temp_buf_paddr = g2d_temp_buf->buf_paddr;

    lv_area_t rel_coords;//relative coord to buf area
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_ctx->clip_area);
    lv_area_move(&rel_clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    bool has_scale = (dsc->zoom != LV_IMG_ZOOM_NONE);
    bool has_rotation = (dsc->angle != 0);
    bool unsup_rotation = false;
    bool has_recolor = (dsc->recolor_opa != LV_OPA_TRANSP);

    lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);
   //blend_area is smalller than src_area
    lv_area_t blend_area;
    if(!has_rotation && !_lv_area_intersect(&blend_area, &rel_coords, &rel_clip_area))
        return; /*Fully clipped, nothing to do*/

    bool has_mask = lv_draw_mask_is_any(&blend_area);
    lv_coord_t src_width = lv_area_get_width(coords);
    lv_coord_t src_height = lv_area_get_height(coords);
    if(has_rotation) {
        /*
         * PXP can only rotate at 90x angles.
         */
        if(dsc->angle % 900) {
            printf("Rotation angle %d is not supported. PXP can rotate only 90x angle.", dsc->angle);
            unsup_rotation = true;
        }

        /*
         * PXP is set to process 16x16 blocks to optimize the system for memory
         * bandwidth and image processing time.
         * The output engine essentially truncates any output pixels after the
         * desired number of pixels has been written.
         * When rotating a source image and the output is not divisible by the block
         * size, the incorrect pixels could be truncated and the final output image
         * can look shifted.
         */
        // (JINGHUI)From here we could know the src_width is the real original src_buf's width. 
        if(src_width % 16 || src_height % 16) {
            printf("Rotation is not supported for image w/o alignment to block size 16x16.");
            unsup_rotation = true;
        }
    }
    // G2D Recolor func is unsupport now because of the blend performace. 
    // But if the image needs to be rotated and recolored, it should be deployed on G2D.
    if(has_mask || has_recolor || has_scale || unsup_rotation || lv_area_get_size(&blend_area) < LV_GPU_NXP_G2D_SIZE_LIMIT|| LV_COLOR_DEPTH != 32) 
    {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }

    char kv_key[20];
    sprintf(kv_key, "%x", src_buf); 
    int src_buf_paddr = get(kv_key);
    lv_point_t pivot = dsc->pivot;
    lv_coord_t piv_offset_x;
    lv_coord_t piv_offset_y;
    lv_area_t src_area;
    lv_area_t rot_src_area;
    lv_coord_t src_stride;

    if(has_rotation){
            enum g2d_rotation g2d_angle;
            switch(dsc->angle) {
            case 0:
                g2d_angle = G2D_ROTATION_0;
                piv_offset_x = 0;
                piv_offset_y = 0;
                break;
            case 900:
                piv_offset_x = pivot.x + pivot.y - src_height;
                piv_offset_y = pivot.y - pivot.x;
                g2d_angle = G2D_ROTATION_270;
                break;
            case 1800:
                piv_offset_x = 2 * pivot.x - src_width;
                piv_offset_y = 2 * pivot.y - src_height;
                g2d_angle = G2D_ROTATION_180;
                break;
            case 2700:
                piv_offset_x = pivot.x - pivot.y;
                piv_offset_y = pivot.x + pivot.y - src_width;
                g2d_angle = G2D_ROTATION_90;
                break;
            default:
                piv_offset_x = 0;
                piv_offset_y = 0;
                g2d_angle = G2D_ROTATION_0;
            }

            lv_area_t rot_rel_coords; 
            lv_area_t rot_coords;

            lv_area_copy(&rot_rel_coords, &rel_coords);
            lv_area_copy(&rot_coords, coords);

            lv_area_move(&rot_rel_coords, piv_offset_x, piv_offset_y);
            lv_area_move(&rot_coords, piv_offset_x, piv_offset_y);
            if (g2d_angle == G2D_ROTATION_270 || g2d_angle == G2D_ROTATION_90){
                rot_rel_coords.x2 = rot_rel_coords.x1 + src_height -1;
                rot_rel_coords.y2 = rot_rel_coords.y1 + src_width -1;
            }
            if(!_lv_area_intersect(&blend_area, &rot_rel_coords, &rel_clip_area))    
                return;

            rot_src_area.x1 = blend_area.x1 - (rot_coords.x1 - draw_ctx->buf_area->x1);
            rot_src_area.y1 = blend_area.y1 - (rot_coords.y1 - draw_ctx->buf_area->y1);
            rot_src_area.x2 = rot_src_area.x1 + lv_area_get_width(&blend_area) - 1;
            rot_src_area.y2 = rot_src_area.y1 + lv_area_get_height(&blend_area) - 1;
            src_stride = lv_area_get_width(&rot_coords);

            switch(dsc->angle) {
            case 0:
                src_area.x1 = rot_src_area.x1;
                src_area.y1 = rot_src_area.y1;
                src_area.x2 = rot_src_area.x2;
                src_area.y2 = rot_src_area.y2;
                break;
            case 900:
                src_area.x1 = rot_src_area.y1;
                src_area.y1 = lv_area_get_width(&rot_rel_coords) - rot_src_area.x2 - 1;
                src_area.x2 = src_area.x1 +  lv_area_get_height(&blend_area) - 1;
                src_area.y2 = src_area.y1 +  lv_area_get_width(&blend_area) - 1;
                break;
            case 1800:
                src_area.x1 = lv_area_get_width(&rot_rel_coords) - rot_src_area.x2 - 1;
                src_area.y1 = lv_area_get_height(&rot_rel_coords) - rot_src_area.y2 - 1;
                src_area.x2 = src_area.x1 +  lv_area_get_width(&blend_area) - 1;
                src_area.y2 = src_area.y1 +  lv_area_get_height(&blend_area) - 1;
                break;
            case 2700:
                src_area.x1 = lv_area_get_height(&rot_rel_coords) - rot_src_area.y2 -1;
                src_area.y1 = rot_src_area.x1;
                src_area.x2 = src_area.x1 +  lv_area_get_height(&blend_area) - 1;
                src_area.y2 = src_area.y1 +  lv_area_get_width(&blend_area) - 1;
                break;
            default:
                src_area.x1 = rot_src_area.x1;
                src_area.y1 = rot_src_area.y1;
                src_area.x2 = rot_src_area.x2;
                src_area.y2 = rot_src_area.y2;
            }
            src_stride = lv_area_get_width(&rel_coords);
            lv_gpu_nxp_g2d_blit_rot(&blend_area, dest_stride, &src_area, src_stride, src_buf_paddr, g2d_angle, dsc->opa);
            return;
        }

        src_area.x1 = blend_area.x1 - (coords->x1 - draw_ctx->buf_area->x1);
        src_area.y1 = blend_area.y1 - (coords->y1 - draw_ctx->buf_area->y1);
        src_area.x2 = src_area.x1 + lv_area_get_width(&blend_area) - 1;
        src_area.y2 = src_area.y1 + lv_area_get_height(&blend_area) - 1;
        src_stride = lv_area_get_width(coords);
        lv_gpu_nxp_g2d_blit(&blend_area, dest_stride, src_buf_paddr, &src_area, src_stride, dsc->opa);

}


#endif /*LV_USE_GPU_NXP_G2D*/
