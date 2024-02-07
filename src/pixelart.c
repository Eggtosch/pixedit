#include "pixelart.h"
#include "raygui.h"
#include "error.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int pixelart_empty(struct palette *palette, int width, int height, struct pixelart *pixelart) {
	if (pixelart == NULL || palette == NULL || width <= 0 || height <= 0) {
		return ERROR_INVALID_ARG;
	}

	if (width > 64 || height > 64) {
		return ERROR_TOO_BIG;
	}

	pixelart->palette = *palette;
	pixelart->width = width;
	pixelart->height = height;
	pixelart->pixels = malloc(width * height);
	memset(pixelart->pixels, 0, width * height);
	return 0;
}

int pixelart_from_file(const char *file, struct pixelart *pixelart) {
	if (file == NULL || pixelart == NULL) {
		return ERROR_INVALID_ARG;
	}

	if (!FileExists(file)) {
		return ERROR_NO_FILE;
	}

	struct palette palette;
	char *text = LoadFileText(file);
	int err = palette_from_pixart_header(text, &palette);
	if (err != 0) {
		return err;
	}

	int width = 0;
	int height = 0;
	char *rest = strchr(text, '\n') + 1;
	sscanf(rest, "%dx%d", &width, &height);
	if (width <= 0 || height <= 0) {
		return ERROR_PARSE;
	} else if (width > 64 || height > 64) {
		return ERROR_TOO_BIG;
	}

	pixelart_empty(&palette, width, height, pixelart);

	char *data = strchr(rest, '\n') + 1;
	for (int y = 0; y < pixelart->height; y++) {
		for (int x = 0; x < pixelart->width; x++) {
			int index = 0;
			sscanf(data, "%02d", &index);
			pixelart_set_pixel(pixelart, x, y, index);
			data += 3;
		}
	}

	return 0;
}

int pixelart_to_file(const char *file, struct pixelart *pixelart) {
	if (file == NULL || pixelart == NULL) {
		return ERROR_INVALID_ARG;
	}

	FILE *f = fopen(file, "w+");
	if (f == NULL) {
		return ERROR_CREATE_FILE;
	}

	int err = palette_to_file(f, &pixelart->palette);
	if (err != 0) {
		return err;
	}

	fprintf(f, "%dx%d\n", pixelart->width, pixelart->height);
	for (int y = 0; y < pixelart->height; y++) {
		for (int x = 0; x < pixelart->width; x++) {
			int index = pixelart_get_pixel(pixelart, x, y);
			fprintf(f, "%02d%c", index, x == pixelart->width - 1 ? '\n' : ',');
		}
	}
	fclose(f);
	return 0;
}

void pixelart_set_pixel(struct pixelart *pixelart, int x, int y, uint8_t index) {
	if (x >= pixelart->width || y >= pixelart->height || index > pixelart->palette.n_colors) {
		return;
	}

	pixelart->pixels[y * pixelart->width + x] = index;
}

uint8_t pixelart_get_pixel(struct pixelart *pixelart, int x, int y) {
	if (x >= pixelart->width || y >= pixelart->height) {
		return 0;
	}

	return pixelart->pixels[y * pixelart->width + x];
}
