#!/bin/sh
# Build the PC port for Monster Hunter Dos (MH2, JP 1.04, VERSION=mh2, docs/mh2_port.md): build/mh2/pc/mhview_2.
# Same game C and platform layer as the G build, plus -DVERSION_MH2; the runtime reads SLPM_662.80 and DATA.BIN,
# with symbols and pointer words made by tools/mh2_pcmeta.py (the ELF is stripped). Runs in build/mh2/pctree, a tree
# of symlinks, so build/pc (MH1) and build/mhg/pctree (G) are left alone.
#   tools/build_pc_mh2.sh && build/mh2/pc/mhview_2 disc/mh2 --boot --shot build/mh2/shot.png --time 10
set -e
cd "$(dirname "$0")/.."
TOP=$PWD
python3 tools/mh2_pcmeta.py
T=build/mh2/pctree
mkdir -p "$T/build"
rm -f "$T"/build/pc/rt_gen.defsym "$T"/build/pc/rt_gen.defsym.*
for d in src include tools third_party config disc; do
    [ -e "$d" ] && ln -sfn "$TOP/$d" "$T/$d"
done
mkdir -p build/mhg
ln -sfn "$TOP/build/mh2" "$T/build/mh2"
ln -sfn "$TOP/build/mhg" "$T/build/mhg"
[ -d build/sysroot32 ] && ln -sfn "$TOP/build/sysroot32" "$T/build/sysroot32"
export MH1_VERSION="$(git rev-parse --short=10 HEAD 2>/dev/null || echo unknown)$(git diff --quiet HEAD 2>/dev/null || echo +)-mh2"
VERSION=mh2 sh "$T/tools/build_pc.sh"
mkdir -p build/mh2/pc
cp "$T/build/pc/mhview" build/mh2/pc/mhview_2
echo "built build/mh2/pc/mhview_2 (Monster Hunter Dos)"
