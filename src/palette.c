#include <stdlib.h>
#include <stdio.h>

#include "palette.h"
#include "raygui.h"
#include "error.h"

/* Palette file format:
 * One color per line, comments start with # until end of line
 * 0xffffffff # comments...\n
 * 0x00000000 # comments...\n
 */
static int parse_palette(const char *palette_str, struct palette *palette) {
	int n_colors = 0;
	int line = 1;
	while (*palette_str) {
		char c = *palette_str;
		if (c == '\n') {
			line++;
		} else if (c == '#') {
			while (*palette_str && *palette_str != '\n') {
				palette_str++;
			}

			line++;
		} else {
			uint32_t color_argb = 0;
			if (sscanf(palette_str, "0x%08x", &color_argb) != 1) {
				if (line == 1 && n_colors > 1) {
					return n_colors;
				}
				printf("Error reading palette file at line %d\n", line);
				return -1;
			}

			if (n_colors == 0 && color_argb != 0) {
				n_colors++;
			}

			palette_str += 10;

			if (palette != NULL) {
				palette->colors[n_colors] = palette_to_raylib_color(color_argb);
			}

			n_colors++;
		}

		palette_str++;
	}

	return n_colors;
}

int palette_from_file(const char *filename, struct palette *palette) {
	if (filename == NULL || palette == NULL) {
		return ERROR_INVALID_ARG;
	}

	if (!FileExists(filename)) {
		return ERROR_NO_FILE;
	}

	char *palette_str = LoadFileText(filename);
	if (palette_str == NULL) {
		return ERROR_READ_FILE;
	}

	int ret = palette_from_pixart_header(palette_str, palette);
	UnloadFileText(palette_str);
	return ret;
}

int palette_from_pixart_header(const char *text, struct palette *palette) {
	if (text == NULL || palette == NULL) {
		return ERROR_INVALID_ARG;
	}

	int n_colors = parse_palette(text, NULL);
	if (n_colors < 0) {
		return ERROR_PARSE;
	} else if (n_colors > 15) {
		return ERROR_TOO_MANY_COLORS;
	}

	palette->n_colors = n_colors;
	palette->colors = malloc(sizeof(Color) * palette->n_colors);
	palette->colors[0] = GetColor(0x00000000);
	parse_palette(text, palette);

	return 0;
}

int palette_to_file(FILE *f, struct palette *palette) {
	if (f == NULL || palette == NULL) {
		return ERROR_INVALID_ARG;
	}

	for (int i = 0; i < palette->n_colors; i++) {
		uint32_t c = palette_from_raylib_color(palette->colors[i]);
		fprintf(f, "0x%08x%c", c, i == palette->n_colors - 1 ? '\n' : ',');
	}

	return 0;
}

Color palette_to_raylib_color(uint32_t color_argb) {
	uint32_t color_rgba = (color_argb >> 24) | (color_argb << 8);
	return GetColor(color_rgba);
}

uint32_t palette_from_raylib_color(Color c) {
	uint32_t color_rgba = ColorToInt(c);
	uint32_t color_argb = (color_rgba >> 8) | (color_rgba << 24);
	return color_argb;
}
