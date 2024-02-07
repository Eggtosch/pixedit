#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "raygui.h"
#include "pixelart.h"
#include "palette.h"
#include "error.h"
#include "editor.h"

static void create_pixart_file(const char *pixart_file, const char *palette_file, int width, int height) {
	struct palette palette;
	int err = palette_from_file(palette_file, &palette);
	if (err != 0) {
		error_print(err);
		return;
	}

	struct pixelart pixelart;
	err = pixelart_empty(&palette, width, height, &pixelart);
	if (err != 0) {
		error_print(err);
		return;
	}

	err = pixelart_to_file(pixart_file, &pixelart);
	if (err != 0) {
		error_print(err);
		return;
	}
}

static void update_pixart_palette(const char *pixart_file, const char *palette_file) {
	struct pixelart pixelart;
	int err = pixelart_from_file(pixart_file, &pixelart);
	if (err != 0) {
		error_print(err);
		return;
	}

	struct palette palette;
	err = palette_from_file(palette_file, &palette);
	if (err != 0) {
		error_print(err);
		return;
	}


	pixelart.palette = palette;

	err = pixelart_to_file(pixart_file, &pixelart);
	if (err != 0) {
		error_print(err);
		return;
	}
}

static void convert_pixart_to_png(const char *pixart_file) {
	struct pixelart pixelart;
	int err = pixelart_from_file(pixart_file, &pixelart);
	if (err != 0) {
		error_print(err);
		return;
	}

	void *data = malloc(sizeof(uint32_t) * pixelart.width * pixelart.height);
	uint8_t *data8 = (uint8_t*) data;
	int i = 0;
	for (int y = 0; y < pixelart.height; y++) {
		for (int x = 0; x < pixelart.width; x++, i += sizeof(uint32_t)) {
			int index = pixelart_get_pixel(&pixelart, x, y);
			uint32_t color_rgba = ColorToInt(pixelart.palette.colors[index]);
			data8[i+0] = (color_rgba >> 24) & 0xff;
			data8[i+1] = (color_rgba >> 16) & 0xff;
			data8[i+2] = (color_rgba >> 8)  & 0xff;
			data8[i+3] = color_rgba & 0xff;
		}
	}

	Image image = {
		.width = pixelart.width,
		.height = pixelart.height,
		.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
		.mipmaps = 1,
		.data = data,
	};

	char *png_filename;
	asprintf(&png_filename, "%s.png", pixart_file);
	ExportImage(image, png_filename);
}

static void usage(const char *prog, const char *error_string) {
	if (error_string != NULL) {
		printf("%s\n", error_string);
	}

	printf("Usage: %s [options] <pixart file>\n", prog);
	printf("Options:\n");
	printf("  --to-png                                  convert a .pixart file to a png\n");
	printf("  --create <palette-file> <width>x<height>  create a new pixart file\n");
	printf("  --update-palette <palette-file>           update the palette of a pixart file\n");
	printf("  --help                                    display this message\n");
	exit(error_string != NULL);
}

int main(int argc, char **argv) {
	SetTraceLogLevel(LOG_NONE);

	const char *pixart_file = NULL;
	const char *palette_file = NULL;
	int pixart_width = 0;
	int pixart_height = 0;
	bool create = false;
	bool update = false;
	bool convert = false;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0) {
			usage(argv[0], NULL);
		} if (strcmp(argv[i], "--create") == 0) {
			if (argc <= i + 2) {
				usage(argv[0], "Not enough arguments to --create");
			}

			create = true;
			palette_file = argv[i+1];
			sscanf(argv[i+2], "%dx%d", &pixart_width, &pixart_height);
			i += 2;
		} else if (strcmp(argv[i], "--update-palette") == 0) {
			if (argc <= i + 1) {
				usage(argv[0], "Not enough arguments to --update-palette");
			}

			update = true;
			palette_file = argv[i+1];
			i++;
		} else if (strcmp(argv[i], "--to-png") == 0) {
			convert = true;
		} else if (pixart_file == NULL) {
			pixart_file = argv[i];
		}
	}

	if (pixart_file == NULL) {
		usage(argv[0], "Missing pixart file argument");
	}

	if (update) {
		update_pixart_palette(pixart_file, palette_file);
		return 0;
	}

	if (create) {
		if (pixart_width <= 0 || pixart_height <= 0) {
			usage(argv[0], "Could not parse correct image dimensions");
		}

		create_pixart_file(pixart_file, palette_file, pixart_width, pixart_height);
		return 0;
	}

	if (convert) {
		convert_pixart_to_png(pixart_file);
		return 0;
	}

	struct pixelart pixelart;
	int err = pixelart_from_file(pixart_file, &pixelart);
	if (err != 0) {
		error_print(err);
		return err;
	}

	err = editor_run(&pixelart);
	if (err != 0) {
		error_print(err);
		return err;
	}

	err = pixelart_to_file(pixart_file, &pixelart);
	if (err != 0) {
		error_print(err);
		return err;
	}

	return 0;
}
