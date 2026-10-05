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
RT="src/pc/rt/rt_mem.c src/pc/rt/rt_flmat.c src/pc/rt/rt_data.c src/pc/rt/rt_game.c src/pc/rt/rt_fl.c src/pc/rt/rt_overlay.c src/pc/rt/rt_main.c src/pc/rt/rt_eft.c"   # (listing only)
# Decompiled game C run natively. set14_nm.c is the whole set14 file
# (set14_trans is a near-match on the PS2 side, believed equivalent).
# stage_set.c (main) spawns each stage's set objects; its calls into the
# overlay go through src/pc/rt/rt_overlay.c. set13_nm.c holds set13_m /
# set13_trans (near-matches on the PS2 side, believed equivalent).
GAME="src/game/set/set14_nm.c src/game/set/set00.c src/main/stage/stage_set.c \
      src/main/set/set13.c src/main/set/set13b.c src/main/set/set13c.c src/main/set/set13_nm.c \
      src/main/hit/hit2.c src/main/hit/hit2c.c \
      src/game/set/set09.c src/game/set/set17.c \
      src/game/set/set03.c src/game/set/set04.c src/game/set/set05_nm.c src/game/set/set07.c src/game/set/set08.c src/game/set/set10.c src/game/set/set11.c src/game/set/set15.c src/game/set/set16.c src/game/set/set18.c src/game/set/set19.c src/game/set/set20_nm.c src/game/set/set22.c \
      src/main/set/set12.c src/main/pl/pl_master_ck.c src/main/stage/trans_stage_nm.c"
# Effects and shells (game.bin eft*/shell*, main eft*). Split files: the
# whole-file _nm.c where it holds every function, else the matching pieces
# plus the _nm.c near-matches. Files in WEAK are near-match copies that
# repeat some matching functions: their symbols are made weak so the
# matching copies win.
EFT="src/game/eft/eft00.c src/main/eft/eft01.c src/main/eft/eft02_nm.c src/game/eft/eft04_nm.c \
     src/game/eft/eft05_nm.c src/main/eft/eft06.c src/main/eft/eft06b.c src/main/eft/eft06_nm.c \
     src/game/eft/eft07.c src/game/eft/eft08.c src/game/eft/eft09.c src/game/eft/eft10.c \
     src/game/eft/eft11_nm.c src/game/eft/eft12.c src/main/eft/eft13.c src/main/eft/eft13b.c \
     src/main/eft/eft13d.c src/main/eft/eft13e.c src/main/eft/eft13_nm.c src/game/eft/eft14.c \
     src/game/eft/eft15.c src/game/eft/eft16_nm.c src/game/eft/eft17.c src/game/eft/eft18_nm.c \
     src/game/eft/eft19.c src/main/eft/eft20.c src/main/eft/eft20c.c src/main/eft/eft20d.c \
     src/main/eft/eft20_nm.c src/game/eft/eft21.c src/game/eft/eft22_nm.c src/game/eft/eft23_nm.c \
     src/game/eft/eft24.c src/main/eft/eft26.c \
     src/game/shell/shell00.c src/game/shell/shell01.c src/game/shell/shell02.c src/game/shell/shell03.c \
     src/game/shell/shell04.c src/game/shell/shell05.c src/game/shell/shell06b.c src/game/shell/shell06_nm.c \
     src/game/shell/shell08.c src/game/shell/shell08b.c src/game/shell/shell08c.c src/game/shell/shell08d.c \
     src/game/shell/shell08_nm.c src/game/shell/shell09.c src/game/shell/shell10.c src/game/shell/shell11.c \
     src/game/shell/shell12.c src/game/shell/shell13.c src/game/shell/shell14.c src/game/shell/shell15.c \
     src/game/shell/shell16.c src/game/shell/shell17.c src/game/shell/shell18.c src/game/shell/shell19.c \
     src/game/shell/shell20.c src/game/shell/shell21.c src/game/shell/shell22_nm.c src/game/shell/shell23.c"
WEAK="shell06_nm eft20_nm"
GAME="$GAME $EFT"

SDL_CFLAGS="-I/usr/include/SDL2 -D_REENTRANT"
CFLAGS="-m32 -std=c99 -O2 -g -Wall -Wextra -Wno-unused-parameter -D_POSIX_C_SOURCE=200809L"
GAMEFLAGS="-m32 -std=gnu99 -O2 -g -fno-strict-aliasing -Iinclude -w"
LIBS="-lSDL2 -lGL -lm -ldl -rdynamic"   # -rdynamic: rt_data.c finds host symbols with dlsym
# unnamed PS2 data the game C refers to as D_<addr>: rows of rview_mat
# (0x3F2060) and two game.bin tables
LIBS="$LIBS -Wl,--defsym,D_3F2080=rview_mat+0x20 -Wl,--defsym,D_3F2090=rview_mat+0x30 \
      -Wl,--defsym,D_63BC40=enemy_shadow_size -Wl,--defsym,D_63BD60=enemy_mahi_size"

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
    b=$(basename "$f" .c)
    o="build/pc/$b.o"
    gcc $GAMEFLAGS $SYS -c "$f" -o "$o"
    case " $WEAK " in *" $b "*) objcopy --weaken "$o" ;; esac
    OBJS="$OBJS $o"
done
# data tables (names in src/pc/rt/tables.txt; bytes come from the disc at run time)
python3 tools/gen_rt_tables.py src/pc/rt/tables.txt build/pc/rt_tables.c
# shellcheck disable=SC2086
gcc $CFLAGS $SYS -c build/pc/rt_tables.c -o build/pc/rt_tables.o
OBJS="$OBJS build/pc/rt_tables.o"
# runtime files that include the game headers
for f in rt_game rt_fl rt_flmat rt_data rt_overlay rt_main rt_eft; do
    # shellcheck disable=SC2086
    gcc $CFLAGS $SYS $SDL_CFLAGS -Iinclude -c src/pc/rt/$f.c -o build/pc/$f.o
    OBJS="$OBJS build/pc/$f.o"
done
# shellcheck disable=SC2086
gcc $CFLAGS $SYS $SDL_CFLAGS $PC src/pc/rt/rt_mem.c $OBJS -o build/pc/mhview $LIBS
echo "built build/pc/mhview (32-bit)"
