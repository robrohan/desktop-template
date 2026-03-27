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

STD:=c11
FILES:=src/wefx.c src/3d.c src/main.c
EXT:=

# Windows mignw32 needs static
STATIC:=
ifeq ($(CC),x86_64-w64-mingw32-gcc)
	STATIC = --static
endif

RAYLIB_LIB := -L./libs/$(PLATFORM)/$(CPU)/ -lraylib
RAYLIB_INC := -I./vendor/raylib/src/

ifeq ($(PLATFORM),Windows)
	LIBS := -ggdb \
		-lshell32 \
		-lwinmm \
		-lgdi32 \
		-lopengl32 \
		-mwindows \
		-mshstk \
		$(RAYLIB_LIB) \
		$(STATIC)
endif
ifeq ($(PLATFORM),Darwin)
	LIBS := -lm \
		-framework Foundation \
		-framework AppKit \
		-framework OpenGL \
		-framework IOKit \
		-framework CoreVideo \
		$(RAYLIB_LIB) \
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
		$(RAYLIB_LIB) \
		$(STATIC)
endif

dummy:
	@echo "make clean clean_libs    - "
	@echo "make fetch               - "
	@echo "make libs                - "
	@echo "make run                 - debug"
	@echo "make build               - release"

fetch:
	curl https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h > ./vendor/miniaudio.h
	curl https://raw.githubusercontent.com/robrohan/r2/refs/heads/main/r2_maths.h > ./vendor/r2_maths.h
	[ -d ./vendor/raylib ] || (curl -L https://github.com/raysan5/raylib/archive/refs/tags/5.5.zip -o /tmp/raylib.zip && unzip -q /tmp/raylib.zip -d /tmp/ && mv /tmp/raylib-5.5 ./vendor/raylib && rm /tmp/raylib.zip)

libs:
	cd ./vendor/raylib/src && make PLATFORM=PLATFORM_DESKTOP
	mkdir -p ./libs/$(PLATFORM)/$(CPU)/
	cp ./vendor/raylib/src/libraylib.a ./libs/$(PLATFORM)/$(CPU)/

clean_libs:
	rm -rf libs

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
		-I./vendor/ $(RAYLIB_INC) \
		-o build/$(PLATFORM)/$(CPU)/$(APP)$(EXT) \
		$(LIBS) \
		-DWEFX_NO_WALLOC \
		-DWEFX_NO_EXPORT \
		-DWEFX_NO_MATH \
		-DWEFX_ORIGIN_TOP_LEFT \
		-DDEBUG_BOX_TRIANGLE
#		-DDEBUG_UV_TRIANGLE

# Do the actual build
build:
	mkdir -p ./build/$(PLATFORM)/$(CPU)/
	$(CC) $(CUSTOM_CFLAGS) $(C_ERRS) $(CFLAGS) -std=$(STD) \
		$(FILES) \
		-I./vendor/ $(RAYLIB_INC) \
		-o build/$(PLATFORM)/$(CPU)/$(APP)$(EXT) \
		$(LIBS) \
		-DNDEBUG \
		-DWEFX_NO_WALLOC \
		-DWEFX_NO_EXPORT \
		-DWEFX_NO_MATH \
		-DWEFX_ORIGIN_TOP_LEFT
	cp ./LICENSE ./build/$(PLATFORM)/LICENSE
