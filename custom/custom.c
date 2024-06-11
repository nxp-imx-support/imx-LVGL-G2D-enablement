// SPDX-License-Identifier: MIT
// Copyright 2020 NXP


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
}

#if LV_USE_GPU_NXP_G2D

#include <string.h>

#define G2D_TEMP_BUF_SIZE HOR_RES * VER_RES * 4U

struct g2d_buf *ctx_buf;
struct g2d_buf *g2d_temp_buf;
void *g2d_handle = NULL;
struct g2d_surface src_surf;
struct g2d_surface dst_surf;

struct g2d_buf *g2d_PM5544_MK10_alpha_640x480_buf;

void g2d_init(){
  if (g2d_open(&g2d_handle)) {
    printf("g2d_open fail.\n");
    return -1;}
  src_surf.format  = G2D_BGRA8888;
  dst_surf.format  = G2D_BGRA8888;
g2d_img_buf_init();
}

void g2d_img_buf_init(){
  g2d_temp_buf = g2d_alloc(G2D_TEMP_BUF_SIZE, 0);
  init_g2d_PM5544_MK10_alpha_640x480_buf(&_PM5544_MK10_alpha_640x480);
}

void g2d_deinit(){
  g2d_close(g2d_handle);
  g2d_img_buf_free();
}

void g2d_img_buf_free(){
  g2d_free(g2d_temp_buf);
  g2d_free(g2d_PM5544_MK10_alpha_640x480_buf);
}

void init_g2d_PM5544_MK10_alpha_640x480_buf(lv_img_dsc_t *_PM5544_MK10_alpha_640x480){
  g2d_PM5544_MK10_alpha_640x480_buf = g2d_alloc(_PM5544_MK10_alpha_640x480->data_size, 0);
  uint8_t *_PM5544_MK10_alpha_640x480_map_temp = (int*)g2d_PM5544_MK10_alpha_640x480_buf->buf_vaddr;
  memcpy(_PM5544_MK10_alpha_640x480_map_temp,_PM5544_MK10_alpha_640x480_map,_PM5544_MK10_alpha_640x480->data_size);
  _PM5544_MK10_alpha_640x480->data = _PM5544_MK10_alpha_640x480_map_temp;
  char addr_buf[20];
  int paddr = g2d_PM5544_MK10_alpha_640x480_buf->buf_paddr;
  sprintf(addr_buf, "%x", g2d_PM5544_MK10_alpha_640x480_buf->buf_vaddr);
  insert(addr_buf, paddr);
}
#else
void img_init(){
  init_img_PM5544_MK10_alpha_640x480(&_PM5544_MK10_alpha_640x480);
}

void init_img_PM5544_MK10_alpha_640x480(lv_img_dsc_t *_PM5544_MK10_alpha_640x480){
_PM5544_MK10_alpha_640x480->data = _PM5544_MK10_alpha_640x480_map;
}

#endif
