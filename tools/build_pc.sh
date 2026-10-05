#!/bin/sh
# Build the PC viewer (src/pc/) into build/pc/mhview. Needs gcc, SDL2 dev
# files (pkg-config sdl2) and OpenGL. See docs/pc.md.
set -e
cd "$(dirname "$0")/.."
mkdir -p build/pc
SRC="src/pc/viewer.c src/pc/fl/fl_model.c src/pc/gfx/gfx_gl.c \
     src/pc/fmt/afs.c src/pc/fmt/melt.c src/pc/fmt/amo.c src/pc/fmt/apx.c \
     src/pc/fmt/ahi.c src/pc/fmt/aan.c src/pc/fmt/hits.c"
# shellcheck disable=SC2086
gcc -std=c99 -O2 -g -Wall -Wextra -Wno-unused-parameter -D_POSIX_C_SOURCE=200809L \
    $(pkg-config --cflags sdl2) $SRC -o build/pc/mhview \
    $(pkg-config --libs sdl2) -lGL -lm
echo "built build/pc/mhview"
