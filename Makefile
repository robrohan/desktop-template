.PHONY: libs build

CC=gcc
APP=example
PLATFORM=linux

C_ERRS += -Wall -Wextra -Wpedantic \
		-Wformat=2 -Wno-unused-parameter -Wshadow \
		-Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
		-Wredundant-decls -Wnested-externs -Wmissing-include-dirs \
		-Wno-unused

dummy:
	@echo "Probably want: make build"

clean:
	rm -f ./libs/*.a
	rm -rf ./build

# Do the actual build
build:
	mkdir -p ./build/$(PLATFORM)
	$(CC) $(C_ERRS) -std=c11 \
		vendor/wefx/wefx.c \
		src/main.c \
		-I./vendor/ \
		-I./vendor/wefx \
		-o build/$(PLATFORM)/$(APP) -lm -fopenmp