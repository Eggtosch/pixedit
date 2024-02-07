#pragma once

#include "raygui.h"
#include "palette.h"

struct pixelart {
	struct palette palette;
	int width;
	int height;
	uint8_t *pixels;
};

int pixelart_empty(struct palette *palette, int width, int height, struct pixelart *pixelart);
int pixelart_from_file(const char *file, struct pixelart *pixelart);
int pixelart_to_file(const char *file, struct pixelart *pixelart);
void pixelart_set_pixel(struct pixelart *pixelart, int x, int y, uint8_t index);
uint8_t pixelart_get_pixel(struct pixelart *pixelart, int x, int y);
