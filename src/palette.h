#pragma once

#include "raygui.h"
#include <stdint.h>
#include <stdio.h>

struct palette {
	Color *colors;
	int n_colors;
};

int palette_from_file(const char *filename, struct palette *palette);
int palette_from_pixart_header(const char *text, struct palette *palette);
int palette_to_file(FILE *f, struct palette *palette);
Color palette_to_raylib_color(uint32_t color_argb);
uint32_t palette_from_raylib_color(Color c);
