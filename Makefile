.PHONY: libs build

CC=gcc
APP=example
PLATFORM=linux

dummy:
	@echo "Probably want: make build"

clean:
	rm -f ./libs/*.a
	rm -rf ./build

# Do the actual build
build:
	mkdir -p ./build/$(PLATFORM)
	$(CC) -Wall -std=c99 \
		vendor/wefx/wefx.c \
		src/main.c \
		-I./vendor/ \
		-I./vendor/wefx \
		-o build/$(PLATFORM)/$(APP) -lm