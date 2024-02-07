#include "editor.h"

int g_screen_width = 800;
int g_screen_height = 600;
struct pixelart *g_pixelart;
int g_selected_color = 0;

static int get_grid_pixel_size(void) {
	int grid_width = g_screen_width / g_pixelart->width;
	int grid_height = g_screen_height / g_pixelart->height;
	return grid_width < grid_height ? grid_width : grid_height;
}

static Rectangle get_grid_dimensions(void) {
	int grid_size = get_grid_pixel_size();
	int grid_width = grid_size * g_pixelart->width;
	int grid_height = grid_size * g_pixelart->height;
	return (Rectangle){0, 0, grid_width, grid_height};
}

static void render_image(void) {
	int grid_size = get_grid_pixel_size();
	Rectangle grid_dims = get_grid_dimensions();
	Vector2 clicked = {0};
	GuiGrid(grid_dims, NULL, grid_size, 2, &clicked);
	if (clicked.x != -1 && clicked.y != -1) {
		int index = g_selected_color;
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			pixelart_set_pixel(g_pixelart, clicked.x, clicked.y, index);
		} else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			pixelart_set_pixel(g_pixelart, clicked.x, clicked.y, 0);
		}
	}

	for (int y = 0; y < g_pixelart->height; y++) {
		for (int x = 0; x < g_pixelart->width; x++) {
			int index = pixelart_get_pixel(g_pixelart, x, y);
			if (index >= g_pixelart->palette.n_colors) {
				continue;
			}

			Color c = g_pixelart->palette.colors[index];
			DrawRectangle(x * grid_size + 1, y * grid_size + 1, grid_size - 2, grid_size - 2, c);
		}
	}
}

static int min(int a, int b) {
	return a < b ? a : b;
}

static void render_palette(void) {
	Rectangle grid_dims = get_grid_dimensions();
	Rectangle palette_dims = grid_dims;

	// put palette to the right of the pixels
	palette_dims.width = g_screen_width - grid_dims.width;
	palette_dims.x = grid_dims.width;
	palette_dims.height /= 2;

	// make palette a bit smaller
	palette_dims.x += palette_dims.width * 0.125;
	palette_dims.y += palette_dims.height * 0.2;
	palette_dims.width *= 0.75;
	palette_dims.height *= 0.75;

	// remove open remainder of grid
	int grid_size = get_grid_pixel_size() / 2;
	if (grid_size < 20) {
		grid_size = 20;
	}

	palette_dims.width -= (int) palette_dims.width % grid_size;
	int n_colors = g_pixelart->palette.n_colors;
	int x_colors = min(palette_dims.width / grid_size, n_colors);
	if (x_colors == 0) {
		return;
	}

	int y_colors = n_colors / x_colors + ((n_colors % x_colors) != 0);
	palette_dims.width = x_colors * grid_size;
	palette_dims.height = y_colors * grid_size;

	Vector2 clicked = {0};
	GuiGrid(palette_dims, NULL, grid_size, 2, &clicked);
	for (int i = 0; i < g_pixelart->palette.n_colors; i++) {
		int x = palette_dims.x + (i % x_colors) * grid_size;
		int y = (int) palette_dims.y + (i / x_colors) * grid_size;
		Color c = g_pixelart->palette.colors[i];
		DrawRectangle(x + 1, y + 1, grid_size - 2, grid_size - 2, c);
	}

	if (clicked.x != -1 && clicked.y != -1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		int i = clicked.y * x_colors + clicked.x;
		if (i >= g_pixelart->palette.n_colors) {
			i = 0;
		}

		g_selected_color = i;
	}

	Vector2 mouse = GetMousePosition();
	DrawRectangle(mouse.x - 10, mouse.y - 10, 10, 10, g_pixelart->palette.colors[g_selected_color]);
}

int editor_run(struct pixelart *pixelart) {
	g_pixelart = pixelart;
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
	InitWindow(g_screen_width, g_screen_height, "pixedit");

	SetTargetFPS(30);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
	while (!WindowShouldClose()) {
		if (IsWindowResized()) {
			g_screen_width = GetScreenWidth();
			g_screen_height = GetScreenHeight();
		}

		BeginDrawing();
		ClearBackground(GetColor(0x181818ff));
		render_image();
		render_palette();
		EndDrawing();
	}

	while (1) {
		BeginDrawing();

		Rectangle bounds = {g_screen_width / 2.0 - 200, g_screen_height / 2.0 - 100, 400, 200};
		int choice = GuiMessageBox(bounds, "Save file", "Do you want to save the file?", "Save;Cancel");
		if (choice == 1) {
			return 0;
		} else if (choice == 2) {
			return 999;
		}

		EndDrawing();
	}

	return 0;
}
