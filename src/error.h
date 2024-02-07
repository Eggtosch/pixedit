#pragma once

enum error {
	ERROR_INVALID_ARG = 1,
	ERROR_NO_FILE,
	ERROR_READ_FILE,
	ERROR_CREATE_FILE,
	ERROR_PARSE,
	ERROR_TOO_MANY_COLORS,
	ERROR_TOO_BIG,
	ERROR_LAST,
};

void error_print(enum error e);
