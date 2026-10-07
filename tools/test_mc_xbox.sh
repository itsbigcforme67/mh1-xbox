#!/bin/sh
# Host check of src/pc/xbox/mc_xbox.c (the Xbox memory card on E:\UDATA): the file is compiled
# on Linux against a small POSIX stand-in for nxdk's winapi (tools/mc_xbox_test/windows.h, E: = a
# temp directory) and a make-save / write / list / read / delete sequence is run. Checks the
# card logic and the UDATA layout; NOT the Xbox itself.
cd "$(dirname "$0")/.."
D=/tmp/mc_xbox_test_disk; rm -rf $D; mkdir -p $D
gcc -w -Itools/mc_xbox_test -Isrc/pc/xbox -o /tmp/mc_xbox_test_bin tools/mc_xbox_test/t.c src/pc/xbox/mc_xbox.c || exit 1
/tmp/mc_xbox_test_bin > /tmp/mc_xbox_test.out || exit 1
grep -q "read 5 'hello'" /tmp/mc_xbox_test.out && grep -q "root listing 1 first 'BISLPM-65495MH'" /tmp/mc_xbox_test.out \
  && [ -f $D/UDATA/4D480001/TitleMeta.xbx ] && [ -f $D/UDATA/4D480001/4D48000100000001/SaveMeta.xbx ] \
  && echo "mc_xbox OK (host shim): UDATA layout, save round trip, meta files hidden from the game" \
  || { echo "mc_xbox FAILED"; cat /tmp/mc_xbox_test.out; exit 1; }
