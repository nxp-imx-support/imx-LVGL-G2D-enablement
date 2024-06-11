CSRCS += lv_draw_g2d.c
CSRCS += lv_draw_g2d_blend.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/g2d
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/g2d

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/g2d"
