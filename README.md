# LVGL-G2D enable for i.MX93 and i.MX MPU 

## 1.Introduction
Here is a example to show the deployment of LVGL PXP acceleration on i.MX93 wayland and DRM display backend. 
It include a SW pack for G2D enable on LVGL source code. It can support calling PXP or 2D GPU in LVGL drawing pipeline. It is beneficial to improve performance and reduce CPU loading. 

The acceleration is mainly for image in LVGL. It can reduce the drawing time especially with rotation and reduce the CPU loading.

Most of the source code are located in g2d folder.

## 2.User Guide
### 2.1 Functions
It include the below function:

Color fill
Alpha blend (not full support)
Image alpha blend
Image alpha blend with rotation 

Color fill : Fill area with a color, with optional opacity.
Alpha blend : Blend rectangular image or buffer (src buf) on another buffer (dst_buf) with effects and opacity. The blend mode is SRC OVER in PorterDuff mode.

On i.MX 93, Wayland will also occupy PXP resources by default. Considering the specific performance and PXP loading issues and alpha blend for all widget is not fully supported, we currently only deploy IMAGE related functions on PXP. 

### 2.2 Platform
In theory, it support not only i.MX93 but also all i.MX platform onli if it support G2D drivers. It will call 2DGPU by G2D on the i.MX MPU such as i.MX8ULP.



## 3.Build Instructions
### 3.1 Clone this repo
```bash
git clone --recurse-submodules [repo_url]
```

### 3.2 Set build environment
Source toolchain, for example 
```bash
source ~/data/toolchain/6.1/environment-setup-armv8a-poky-linux
```

### 3.3 Build Steps
```bash
cp -r g2d lvgl/src/draw/nxp
cp -r protocols/ lv_drivers/wayland/
patch -p1 < demo.patch
make clean && make -j
```
Copy the bin file lvgl_demo to your broad.

### 3.4 Steps to port GUI-GUDER project 
How to enable pxp acceleration to GUI-GUIDER project.

1. Clone this repo and set build environment.
2. copy the custom.c and custom.h from your GUI-GUIDER project to custom/ (cover them).
3. detele the generated folder and copy the generated folder from your GUI-GUIDER project.
4. python3 convert.py, it helps to modify the configuration about G2D source code.
5. follow the 3.3 Build Steps.



