CC := gcc
LD := gcc

BINARY      := pixedit
CFLAGS      := -O2 -flto -pipe -Isrc -Wall -Wextra -MMD -MP
LIBRARIES   := -lraylib -lm
LDFLAGS     := -flto

SOURCES     := src/main.c src/editor.c src/palette.c src/pixelart.c src/raygui.c src/error.c
HEADER_DEPS := $(SOURCES:%.c=%.d)
OBJECTS     := $(SOURCES:%.c=%.o)

FS_BINARY_PATH      := ~/.local/bin/$(BINARY)
FS_COMPLETIONS_PATH := ~/.local/share/bash-completion/completions/$(BINARY)

$(BINARY): $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $(BINARY) $(LIBRARIES)

src/raygui.o: src/raygui.c src/raygui.h
	$(CC) $(CFLAGS) -Wno-unused-parameter -c $< -o $@

-include $(HEADER_DEPS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: $(BINARY)
	cp $(BINARY) $(FS_BINARY_PATH)
	cp bash-completion/$(BINARY) $(FS_COMPLETIONS_PATH)

uninstall:
	rm -f $(FS_BINARY_PATH)
	rm -f $(FS_COMPLETIONS_PATH)

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(HEADER_DEPS)
	rm -f $(BINARY)
