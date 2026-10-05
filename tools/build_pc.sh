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
RT="src/pc/rt/rt_mem.c src/pc/rt/rt_flmat.c src/pc/rt/rt_data.c src/pc/rt/rt_game.c src/pc/rt/rt_fl.c src/pc/rt/rt_overlay.c src/pc/rt/rt_main.c"   # (listing only)
# Decompiled game C run natively. set14_nm.c is the whole set14 file
# (set14_trans is a near-match on the PS2 side, believed equivalent).
# stage_set.c (main) spawns each stage's set objects; its calls into the
# overlay go through src/pc/rt/rt_overlay.c. set13_nm.c holds set13_m /
# set13_trans (near-matches on the PS2 side, believed equivalent).
GAME="src/game/set/set14_nm.c src/game/set/set00.c src/main/stage/stage_set.c \
      src/main/set/set13.c src/main/set/set13b.c src/main/set/set13c.c src/main/set/set13_nm.c \
      src/main/hit/hit2.c src/main/hit/hit2c.c"

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
# runtime files that include the game headers
for f in rt_game rt_fl rt_flmat rt_data rt_overlay rt_main; do
    # shellcheck disable=SC2086
    gcc $CFLAGS $SYS $SDL_CFLAGS -Iinclude -c src/pc/rt/$f.c -o build/pc/$f.o
    OBJS="$OBJS build/pc/$f.o"
done
# shellcheck disable=SC2086
gcc $CFLAGS $SYS $SDL_CFLAGS $PC src/pc/rt/rt_mem.c $OBJS -o build/pc/mhview $LIBS
echo "built build/pc/mhview (32-bit)"
