# Copyright 2024 NXP
# NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
# accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
# activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
# terms, then you may not retain, install, activate or otherwise use the software.

# Makefile
#  make clean && make -j$(nproc) RUN_JOBS=-j$(nproc)

CC ?= gcc
LVGL_DIR_NAME = lvgl
LVGL_DIR = ${shell pwd}
PRJ_DIR = ${shell pwd}
CFLAGS = -g3 -ggdb -O3  -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wmissing-prototypes -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wno-discarded-qualifiers -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare \
	-I $(LVGL_DIR)\
	-I $(LVGL_DIR)/lvgl/ \
	-I $(LVGL_DIR)/lv_drivers/wayland/ \
	-I $(LVGL_DIR)/lv_drivers/display/drm/ \
	-I $(LVGL_DIR)/custom/ \
	-I $(LVGL_DIR)/generated/ \
	-I $(LVGL_DIR)/generated/images \
	-I $(SDKTARGETSYSROOT)/usr/include/drm/

export LDFLAGS +=-lm -ldrm -lwayland-client -lxkbcommon -lwayland-cursor -lg2d\

BIN = lvgl_demo

RUN_JOBS = 

#Collect the files to compile
MAINSRC = ./main.c

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
include $(LVGL_DIR)/generated/generated.mk
include $(LVGL_DIR)/custom/custom.mk

OBJEXT ?= .o

CSRCS += $(GEN_CSRCS)

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)


all: default

%.o: %.c
	@$(CC)  $(CFLAGS)  -c $< -o $@
	@echo "CC $<"
    
default: $(AOBJS) $(COBJS) $(MAINOBJ)
	$(CC) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS)
	mkdir -p $(LVGL_DIR)/obj
	mv *.o $(LVGL_DIR)/obj/

clean: 
	rm -f $(BIN) $(AOBJS) $(COBJS) $(MAINOBJ) ./bin/* ./obj/*


