#!/bin/sh
# Build the PC port as a 32-bit ARM (armhf) program on a 64-bit ARM Linux
# box without root, e.g. the Armbian RK3518 box (docs/pc.md "ARM").
# Needs ARMROOT (default ~/mh1arm) prepared as described there: sysroot/
# (armhf runtime + dev packages unpacked) and cross/ (the Debian
# gcc-14-arm-linux-gnueabihf cross compiler unpacked).
# Run the result with tools/run_arm.sh.
set -e
R=${ARMROOT:-$HOME/mh1arm}
S=$R/sysroot; X=$R/cross; A=arm-linux-gnueabihf
export LD_LIBRARY_PATH=$X/usr/lib/aarch64-linux-gnu      # the cross binutils' own libraries
export CC="$X/usr/bin/$A-gcc-14 -isystem $X/usr/$A/include -I$S/usr/include/$A -I$S/usr/include \
 -I$S/usr/include/SDL2 -I$S/usr/include/$A/SDL2 -B$X/usr/$A/lib -L$X/usr/$A/lib -L$S/usr/lib/$A -L$S/lib/$A \
 -Wl,-rpath-link,$S/usr/lib/$A -Wl,-rpath-link,$S/lib/$A -Wl,-rpath-link,$S/usr/lib/$A/pulseaudio \
 -Wl,--dynamic-linker=/lib/ld-linux-armhf.so.3"
export M32="" PC_SYS=" " SDL_CFLAGS="-D_REENTRANT"
export OBJCOPY=$X/usr/bin/$A-objcopy NM=$X/usr/bin/$A-nm
# PS2 (MWCC) char is signed; ARM's default is unsigned
export GAME_EXTRA="-fsigned-char -fpermissive"   # gcc 14 rejects implicit declarations the x86 gcc 13 build accepts
exec "$(dirname "$0")/build_pc.sh"
