.PHONY: libs build

CC:=gcc
APP:=example

PLATFORM:=$(shell uname -s)
CPU:=$(shell uname -m)

C_ERRS += -Wall -Wextra -Wpedantic \
		-Wformat=2 -Wno-unused-parameter -Wshadow \
		-Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
		-Wredundant-decls -Wnested-externs -Wmissing-include-dirs \
		-Wno-unused

STD:=c99
FILES:=src/wefx.c src/3d.c src/main.c
EXT:=

# Windows mignw32 needs static
STATIC:=
ifeq ($(CC),x86_64-w64-mingw32-gcc)
	STATIC = --static
endif

ifeq ($(PLATFORM),Windows)
	LIBS := -ggdb \
		-lshell32 \
		-lwinmm \
		-lgdi32 \
		-lopengl32 \
		-mwindows \
		-mshstk \
		$(STATIC)
endif
ifeq ($(PLATFORM),Darwin)
	LIBS := -lm \
		-framework Foundation \
		-framework AppKit \
		-framework OpenGL \
		-framework CoreVideo \
		-framework IOKit \
		-mshstk \
		$(STATIC)
endif
ifeq ($(PLATFORM),Linux)
	LIBS := -lXrandr \
		-lX11 \
		-lm \
		-lGL \
		-ldl \
		-lpthread \
		-D_POSIX_C_SOURCE=200112L \
		-mshstk \
		$(STATIC)
endif

dummy:
	@echo "You probably want: make build, or make fetch"

fetch:
	curl https://raw.githubusercontent.com/ColleagueRiley/RGFW/refs/heads/main/RGFW.h > ./vendor/RGFW.h
	curl https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h > ./vendor/miniaudio.h

clean:
	rm -rf ./build

convert:
	convert -resize 8x8 \
		./assets/a.png \
		./assets/8x8_2.png

# Develop build
run:
	mkdir -p ./build/$(PLATFORM)/$(CPU)/
	$(CC) $(CUSTOM_CFLAGS) $(C_ERRS) $(CFLAGS) -ggdb -std=$(STD) \
		$(FILES) \
		-I./vendor/ \
		-o build/$(PLATFORM)/$(CPU)/$(APP)$(EXT) \
		$(LIBS) \
		-DWEFX_NO_WALLOC \
		-DWEFX_NO_EXPORT \
		-DWEFX_NO_MATH \
		-DWEFX_ORIGIN_TOP_LEFT \
		-DRENDER_OPENGL \
		-DDEBUG_BOX_TRIANGLE
#		-DDEBUG_UV_TRIANGLE

# Do the actual build
build:
	mkdir -p ./build/$(PLATFORM)/$(CPU)/
	$(CC) $(CUSTOM_CFLAGS) $(C_ERRS) $(CFLAGS) -std=$(STD) \
		$(FILES) \
		-I./vendor/ \
		-o build/$(PLATFORM)/$(CPU)/$(APP)$(EXT) \
		$(LIBS) \
		-DNDEBUG \
		-DWEFX_NO_WALLOC \
		-DWEFX_NO_EXPORT \
		-DWEFX_NO_MATH \
		-DWEFX_ORIGIN_TOP_LEFT \
		-DRENDER_OPENGL
	cp ./LICENSE ./build/$(PLATFORM)/LICENSE
