#!/bin/sh
# Build the PC port for PS2 Monster Hunter G (VERSION=mhg, docs/mhg.md section 4):
# build/mhg/pc/mhview_g. Same game C and platform layer as the MH1 build; the runtime
# reads G's SLPM_658.69 and overlays, with G's symbols and pointer words made by
# tools/mhg_pcmeta.py (G's ELF is stripped). The build runs in build/mhg/pctree, a tree
# of symlinks to src/ include/ tools/ ..., so build/pc (the MH1 build) is left alone.
#   tools/build_pc_mhg.sh && build/mhg/pc/mhview_g disc/mhg --shot build/mhg/shot.png
set -e
cd "$(dirname "$0")/.."
TOP=$PWD
python3 tools/mhg_pcmeta.py
T=build/mhg/pctree
mkdir -p $T/build
# the last build's generated aliases (rt_gen.defsym, reused by build_pc.sh to save compiling) went stale when
# G code started defining one of them itself: "multiple definition of D_3F360A" until a second run. G changes
# fast, so start from none (a few seconds more per build)
rm -f $T/build/pc/rt_gen.defsym $T/build/pc/rt_gen.defsym.*
for d in src include tools third_party config disc; do
    [ -e "$d" ] && ln -sfn "$TOP/$d" $T/$d
done
ln -sfn "$TOP/build/mhg" $T/build/mhg
[ -d build/sysroot32 ] && ln -sfn "$TOP/build/sysroot32" $T/build/sysroot32
export MH1_VERSION="$(git rev-parse --short=10 HEAD 2>/dev/null || echo unknown)$(git diff --quiet HEAD 2>/dev/null || echo +)-mhg"
VERSION=mhg sh $T/tools/build_pc.sh
mkdir -p build/mhg/pc
if [ -n "$ONLINE" ]; then     # ONLINE=1: G's online build (docs/network.md "G online"), build/mhg/pc/mhview_g_online
    cp $T/build/pc/mhview_online build/mhg/pc/mhview_g_online
    echo "built build/mhg/pc/mhview_g_online (PS2 Monster Hunter G, online)"
else
    cp $T/build/pc/mhview build/mhg/pc/mhview_g
    echo "built build/mhg/pc/mhview_g (PS2 Monster Hunter G)"
fi
