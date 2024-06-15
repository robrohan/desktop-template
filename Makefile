.PHONY: libs build

CC=gcc
APP=example
PLATFORM=Linux

C_ERRS += -Wall -Wextra -Wpedantic \
		-Wformat=2 -Wno-unused-parameter -Wshadow \
		-Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
		-Wredundant-decls -Wnested-externs -Wmissing-include-dirs \
		-Wno-unused

DEBUG = -ggdb
STATIC =
# Windows mignw32 needs static
ifeq ($(CC),x86_64-w64-mingw32-gcc)
	STATIC = --static
endif

ifeq ($(PLATFORM),Windows)
	LIBS := -ggdb \
		-lshell32 \
		-lwinmm \
		-lgdi32 \
		-lopengl32 \
		$(STATIC)
	EXT = .exe
endif
ifeq ($(PLATFORM),Darwin)
	LIBS := -lm \
		-framework Foundation \
		-framework AppKit \
		-framework OpenGL \
		-framework CoreVideo \
		$(STATIC)
	EXT = 
endif
ifeq ($(PLATFORM),Linux)
	LIBS := -lXrandr \
		-lX11 \
		-lm \
		-lGL \
		-ldl \
		-lpthread \
		$(STATIC)
	EXT =
endif


dummy:
	@echo "Probably want: make build"

clean:
	rm -f ./libs/*.a
	rm -rf ./build

# Do the actual build
build:
	mkdir -p ./build/$(PLATFORM)
	$(CC) $(CUSTOM_CFLAGS) $(C_ERRS) $(CFLAGS) $(DEBUG) -std=c99 \
		vendor/wefx/wefx.c \
		src/main.c \
		-I./vendor/ \
		-I./vendor/wefx \
		-o build/$(PLATFORM)/$(APP)$(EXT) \
		$(LIBS)
