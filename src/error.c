#include <stdio.h>

#include "error.h"

static const char *errors[] = {
	[ERROR_INVALID_ARG] = "Argument was NULL or invalid\n",
	[ERROR_NO_FILE] = "File does not exist\n",
	[ERROR_READ_FILE] = "Could not read file\n",
	[ERROR_CREATE_FILE] = "Could not create file\n",
	[ERROR_PARSE] = "Could not parse file\n",
	[ERROR_TOO_MANY_COLORS] = "Too many colors in palette, max is 15\n",
	[ERROR_TOO_BIG] = "Pixelart too big, max is 64x64\n",
};

void error_print(enum error e) {
	int err = e;
	if (err >= 1 && err <= ERROR_LAST) {
		printf("%s", errors[err]);
	}
}
