#!/bin/sh
# Make `gcc -m32` usable without root, for the 32-bit PC host (docs/pc.md).
# Downloads the Ubuntu multilib packages (libc6-dev-i386, lib32gcc-13-dev)
# with `apt-get download` (no root) and unpacks them into build/sysroot32.
# The 32-bit runtime libraries (libc6:i386, libsdl2-2.0-0:i386, libgl1:i386)
# must already be installed. With root, this whole script is the same as
#   sudo apt install gcc-multilib
set -e
cd "$(dirname "$0")/.."
SR="$PWD/build/sysroot32"
mkdir -p "$SR/debs"
cd "$SR/debs"
[ -n "$(ls *.deb 2>/dev/null)" ] || apt-get download libc6-dev-i386 lib32gcc-13-dev
for d in *.deb; do dpkg-deb -x "$d" "$SR"; done
# The linker scripts name absolute /usr/lib32 and /lib32 paths: point them at
# the unpacked static parts and the installed i386 runtime instead.
L="$SR/usr/lib32"
RT=/usr/lib/i386-linux-gnu
cat > "$L/libc.so" <<EOS
OUTPUT_FORMAT(elf32-i386)
GROUP ( $RT/libc.so.6 "$L/libc_nonshared.a" AS_NEEDED ( /lib/ld-linux.so.2 ) )
EOS
ln -sf $RT/libm.so.6 "$L/libm.so"
mkdir -p "$SR/lib"; ln -sf $RT/libSDL2-2.0.so.0 "$SR/lib/libSDL2.so"; ln -sf $RT/libGL.so.1 "$SR/lib/libGL.so"
echo "sysroot ready: $SR"
