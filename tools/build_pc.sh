#!/bin/sh
# Build the PC port (src/pc/ + the decompiled game C it runs) into
# build/pc/mhview. 32-bit (gcc -m32): the game C keeps pointers in u32
# fields and its structs must keep their PS2 offsets (docs/pc.md).
# Needs gcc, 32-bit libc/SDL2/GL runtime libraries, and either gcc-multilib
# or the no-root sysroot from tools/setup_pc32.sh.
set -e
cd "$(dirname "$0")/.."
mkdir -p build/pc
PC="src/pc/viewer.c src/pc/fl/fl_model.c src/pc/gfx/gfx_gl.c \
    src/pc/fmt/afs.c src/pc/fmt/melt.c src/pc/fmt/amo.c src/pc/fmt/apx.c \
    src/pc/fmt/ahi.c src/pc/fmt/aan.c src/pc/fmt/hits.c"
RT="src/pc/rt/rt_mem.c src/pc/rt/rt_data.c src/pc/rt/rt_game.c src/pc/rt/rt_fl.c"
# Decompiled game C run natively. set14_nm.c is the whole set14 file
# (set14_trans is a near-match on the PS2 side, believed equivalent).
GAME="src/game/set/set14_nm.c"

SDL_CFLAGS="-I/usr/include/SDL2 -D_REENTRANT"
CFLAGS="-m32 -std=c99 -O2 -g -Wall -Wextra -Wno-unused-parameter -D_POSIX_C_SOURCE=200809L"
GAMEFLAGS="-m32 -std=gnu99 -O2 -g -fno-strict-aliasing -Iinclude -w"
LIBS="-lSDL2 -lGL -lm"

if echo 'int main(void){return 0;}' | gcc -m32 -x c - -o build/pc/.m32test $LIBS 2>/dev/null; then
    SYS=""                                   # gcc-multilib installed
else
    SR=build/sysroot32      # relative: the checkout path may contain spaces
    G="$SR/usr/lib/gcc/x86_64-linux-gnu/13/32"
    [ -d "$SR/usr/lib32" ] || { echo "no 32-bit toolchain: run tools/setup_pc32.sh (or apt install gcc-multilib)"; exit 1; }
    SYS="-idirafter /usr/include/x86_64-linux-gnu -idirafter $SR/usr/include/x86_64-linux-gnu \
         -B$SR/usr/lib32 -B$G -L$SR/usr/lib32 -L$G -L$SR/lib"
fi
rm -f build/pc/.m32test

# shellcheck disable=SC2086
for f in $GAME; do
    o="build/pc/$(basename "$f" .c).o"
    gcc $GAMEFLAGS $SYS -c "$f" -o "$o"
    OBJS="$OBJS $o"
done
# shellcheck disable=SC2086
gcc $CFLAGS $SYS $SDL_CFLAGS -Iinclude -c src/pc/rt/rt_game.c -o build/pc/rt_game.o
# shellcheck disable=SC2086
gcc $CFLAGS $SYS $SDL_CFLAGS -Iinclude -c src/pc/rt/rt_fl.c -o build/pc/rt_fl.o
# shellcheck disable=SC2086
gcc $CFLAGS $SYS $SDL_CFLAGS -Iinclude -c src/pc/rt/rt_data.c -o build/pc/rt_data.o
# shellcheck disable=SC2086
gcc $CFLAGS $SYS $SDL_CFLAGS $PC src/pc/rt/rt_mem.c build/pc/rt_game.o build/pc/rt_fl.o build/pc/rt_data.o \
    $OBJS -o build/pc/mhview $LIBS
echo "built build/pc/mhview (32-bit)"
