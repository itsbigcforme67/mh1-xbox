#!/bin/sh
# Tests of the PS2 save containers and the import / export (agent C, 8 Oct 2026; a few seconds).
# Builds tools/save_test/t.c with the real src/pc/fmt/ps2save.c + lzari.c + src/pc/rt/rt_save.c
# (address and undefined-behaviour sanitizers on) and runs it. The test builds its own made-up
# save in every container (.psu .max .cbs .sps .xps .ps2) and needs no game data or real save.
# Optional: SAVE_DIR=/path/to/memcard0/BISLPM-65495MH's parent to use a real PC save as the base.
cd "$(dirname "$0")/.."
B=build/save_test; mkdir -p $B
gcc -std=gnu99 -g -O1 -Wall -Wextra -Wno-unused-result -Wno-unused-parameter -fsanitize=address,undefined \
    -o $B/t tools/save_test/t.c src/pc/fmt/ps2save.c src/pc/fmt/lzari.c src/pc/rt/rt_save.c || exit 1
$B/t $SAVE_DIR
