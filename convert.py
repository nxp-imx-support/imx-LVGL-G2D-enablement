# /*
# * Copyright 2024 NXP
# * NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
# * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
# * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
# * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
# * terms, then you may not retain, install, activate or otherwise use the software.
# */

import os

def getfiles():
    filenames=os.listdir(r'./generated/images/')
    filenames.remove("images.mk")
    # for filename in filenames:
    for i in range(len(filenames)):
    	filenames[i] = filenames[i][:-2]
    print(filenames)
    return filenames


filenames = getfiles()


#module images
for filename in filenames:
	filepath = "./generated/images/" + filename +".c"
	with open(filepath, 'r') as file:
		lines = file.readlines()
	with open(filepath, "w") as file:
		for line in lines:
			if "lv_img_dsc_t" in line:
				line = line[6:]
			elif ".data = " in line:
				line = "//" + line
			else:
				line = line
			file.write(line)

##module custom.c
file = open("custom/custom.c", mode = "a")
file.write("#if LV_USE_GPU_NXP_G2D\n\n")	
file.write("#include <string.h>\n\n")
file.write("#define G2D_TEMP_BUF_SIZE HOR_RES * VER_RES * 4U\n\n")
file.write("struct g2d_buf *ctx_buf;\n")
file.write("struct g2d_buf *g2d_temp_buf;\n")
file.write("void *g2d_handle = NULL;\n")
file.write("struct g2d_surface src_surf;\n")
file.write("struct g2d_surface dst_surf;\n\n")

for filename in filenames:
	strline = "struct g2d_buf *g2d"+filename+"_buf;\n"
	file.write(strline)
file.write("\n")

file.write("void g2d_init(){\n")
file.write("  if (g2d_open(&g2d_handle)) {\n")
file.write("    printf(\"g2d_open fail.\\n\");\n")
file.write("    return -1;}\n")
file.write("  src_surf.format  = G2D_BGRA8888;\n")
file.write("  dst_surf.format  = G2D_BGRA8888;\n")
file.write("g2d_img_buf_init();\n")
file.write("}\n\n")

file.write("void g2d_img_buf_init(){\n")
file.write("  " + "g2d_temp_buf = g2d_alloc(G2D_TEMP_BUF_SIZE, 0);\n")
for filename in filenames:
	strline = "  " + "init_g2d"+filename+"_buf(&" + filename + ");\n"
	file.write(strline)
file.write("}\n\n")

file.write("void g2d_deinit(){\n")
file.write("  g2d_close(g2d_handle);\n")
file.write("  g2d_img_buf_free();\n")
file.write("}\n\n")

file.write("void g2d_img_buf_free(){\n")
file.write("  " + "g2d_free(g2d_temp_buf);\n")
for filename in filenames:
	strline = "  " + "g2d_free(g2d"+filename+"_buf);\n"
	file.write(strline)
file.write("}\n\n")


init_g2d_buf_funcs = [0] * 10
for filename in filenames:
	g2d_buf = "g2d"+ filename + "_buf"
	img_struct = "lv_img_dsc_t *" + filename
	img_ptr = filename + "_map_temp"

	init_g2d_buf_funcs[0] = "void init_" + g2d_buf + "(" + img_struct + "){\n"
	init_g2d_buf_funcs[1] = "  " + g2d_buf + " = g2d_alloc(" + filename + "->data_size, 0);\n"
	init_g2d_buf_funcs[2] = "  " + "uint8_t *" + img_ptr + " = (int*)" + g2d_buf + "->buf_vaddr;\n"
	init_g2d_buf_funcs[3] = "  " + "memcpy(" + img_ptr + "," + filename + "_map," + filename + "->data_size);\n"
	init_g2d_buf_funcs[4] = "  " + filename + "->data = " + img_ptr + ";\n"
	init_g2d_buf_funcs[5] = "  " + "char addr_buf[20];\n"
	init_g2d_buf_funcs[6] = "  " + "int paddr = " + g2d_buf + "->buf_paddr;\n"
	init_g2d_buf_funcs[7] = "  " + "sprintf(addr_buf, \"%x\", " + g2d_buf + "->buf_vaddr);\n"
	init_g2d_buf_funcs[8] = "  " + "insert(addr_buf, paddr);\n"
	init_g2d_buf_funcs[9] = "}\n"

	for init_g2d_buf_func in init_g2d_buf_funcs:
		file.write(init_g2d_buf_func)
file.write("#else\n")

file.write("void img_init(){\n")
for filename in filenames:
	strline = "  init_img" + filename + "(&" + filename + ");\n"
	file.write(strline)
file.write("}\n\n")
init_img_funcs = [0] * 3
for filename in filenames:
	init_img_funcs[0] = "void init_img" + filename + "(lv_img_dsc_t *" + filename +"){\n"
	init_img_funcs[1] = filename + "->data = " + filename + "_map;\n"
	init_img_funcs[2] = "}\n\n"
	for init_img_func in init_img_funcs:
		file.write(init_img_func)

file.write("#endif\n")
file.close()


file = open("custom/custom.h", mode = "a")
file.write("#include \"lv_conf.h\"\n")
for filename in filenames:
	img_map = "extern uint8_t " + filename + "_map[];\n"
	file.write(img_map)
file.write("#if LV_USE_GPU_NXP_G2D\n")
file.write("#include \"g2d.h\"\n")
file.write("#include \"kv.h\"\n")
file.write("void g2d_init();\n")
file.write("#else\n")
file.write("void img_init();\n")
file.write("#endif\n")

