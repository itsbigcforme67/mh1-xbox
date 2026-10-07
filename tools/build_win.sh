#!/bin/sh
# Cross-build the PC port for 32-bit Windows (i686) on Linux, no root:
# build/win/mhview.exe + SDL2.dll + play.bat (+ the helper scripts), by
# running tools/build_pc.sh with llvm-mingw's clang (the way build_arm.sh
# does for ARM). Needs, outside the repo (docs/pc.md "Windows"):
#   ~/mh1win/llvm-mingw-*-ucrt-*/            github.com/mstorsjo/llvm-mingw releases
#   ~/mh1win/SDL2-2.x.y/i686-w64-mingw32/    SDL2-devel-2.x.y-mingw.tar.gz from libsdl-org/SDL
# (WINROOT overrides ~/mh1win). The build runs in build/win/tree, a tree of
# symlinks to src/ include/ tools/ ..., so that build/pc (the Linux build)
# is left alone.
set -e
cd "$(dirname "$0")/.."
TOP=$PWD
W=${WINROOT:-$HOME/mh1win}
LLVM=$(ls -d "$W"/llvm-mingw-*${WINCRT:-msvcrt}* 2>/dev/null | grep -v "\.tar" | head -1)
SDL=$(ls -d "$W"/SDL2-[0-9]* 2>/dev/null | grep -v '\.tar' | tail -1)/i686-w64-mingw32
[ -x "$LLVM/bin/i686-w64-mingw32-clang" ] || { echo "no llvm-mingw in $W (docs/pc.md, Windows)"; exit 1; }
[ -f "$SDL/lib/libSDL2.dll.a" ] || { echo "no SDL2 mingw development package in $W (docs/pc.md, Windows)"; exit 1; }
T=build/win/tree
mkdir -p $T/build
for d in src include tools third_party config disc; do
    [ -e "$d" ] && ln -sfn "$TOP/$d" $T/$d
done
# the build's version string (git hash) is baked in by build_pc.sh through MH1_VERSION
export MH1_VERSION="$(git rev-parse --short=10 HEAD 2>/dev/null || echo unknown)$(git diff --quiet HEAD 2>/dev/null || echo +)"
export PATH="$LLVM/bin:$PATH"
# clang 18 makes these gcc warnings errors in C99; the decompiled C relies on them
RELAX="-Wno-error=implicit-function-declaration -Wno-error=implicit-int -Wno-error=int-conversion \
 -Wno-error=incompatible-function-pointer-types -Wno-error=return-type -Wno-error=incompatible-pointer-types"
export CC="i686-w64-mingw32-clang"
export M32="" PC_SYS=" " EXE=.exe SYMTAB_ARGS="--prefix _" SYM_PREFIX=_
export LINK1_OPTS="-Wl,--error-limit=0" LINK1_TOLERANT=1
export OBJCOPY=llvm-objcopy NM=llvm-nm
export SDL_CFLAGS="-I$SDL/include/SDL2 -DSDL_MAIN_HANDLED"
export LIBS="-L$SDL/lib -lSDL2 -lopengl32 -lcomdlg32 -lshell32 -lwinmm -lm"
export GAME_EXTRA="$RELAX -w -fno-builtin"   # as nxdk-cc: the game headers declare memset() K&R
export EXTRA_CFLAGS="-DMH1_WIN -include src/pc/rt/win_utf8.h" GAME_NOAGG=""
sh $T/tools/build_pc.sh
mkdir -p build/win
cp $T/build/pc/mhview.exe build/win/mhview.exe
cp "$SDL/bin/SDL2.dll" build/win/SDL2.dll
for f in play.bat bug_report.bat bug_report.ps1; do [ -f tools/win/$f ] && cp tools/win/$f build/win/$f; done
echo "built build/win/mhview.exe (32-bit Windows; SDL2.dll next to it)"
