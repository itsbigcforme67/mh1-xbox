#!/bin/sh
# Run the armhf build (tools/build_arm.sh) through the sysroot's loader, so
# nothing has to be installed system-wide. Arguments go to mhview, e.g.
#   tools/run_arm.sh disc/mh1 --play --quest 10 --size 960x720
R=${ARMROOT:-$HOME/mh1arm}
S=$R/sysroot; A=arm-linux-gnueabihf
cd "$(dirname "$0")/.."
export LIBGL_DRIVERS_PATH=$S/usr/lib/$A/dri
export __EGL_VENDOR_LIBRARY_DIRS=$S/usr/share/glvnd/egl_vendor.d
export DISPLAY=${DISPLAY:-:0}
exec "$S/usr/lib/$A/ld-linux-armhf.so.3" --library-path "$S/usr/lib/$A:$S/lib/$A:$S/usr/lib/$A/pulseaudio" \
    build/pc/mhview "$@"
