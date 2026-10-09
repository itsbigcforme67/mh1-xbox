#!/bin/sh
# End-to-end check of the PS2 save import/export on the PC build (agent C, 8 Oct 2026; ~1.5 min):
# runs test_quest_loop.sh (the game itself writes a save with 1550z), exports that save as .psu .max
# .cbs .sps .xps .ps2 with `mhview --export-save`, wipes the card folder, imports each with
# `--import-save`, and requires that CONTINUE in the game shows 1550z again. Needs disc/mh1 like the
# other game tests. RUN=wine BIN=build/win/mhview.exe works too.
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview}
[ -n "$SKIP_LOOP" ] || sh tools/test_quest_loop.sh >/dev/null 2>&1
OUT=build/show/loop; export MH1_SAVE_DIR="$PWD/$OUT/card"; export RT_NOMOVIE=1 RT_NO_GUI=1
KEEP=$OUT/card_orig; rm -rf $KEEP; cp -r "$MH1_SAVE_DIR" $KEEP
[ -f $KEEP/BISLPM-65495MH/BISLPM-65495MH ] || { echo "no save from the loop test"; exit 1; }
rc=0
for e in psu max cbs sps xps ps2; do
  F=$OUT/exp.$e; rm -f $F
  $RUN $BIN --export-save $F >/dev/null 2>&1 || $RUN $BIN --export-save $F || rc=1
  rm -rf "$MH1_SAVE_DIR" "$MH1_SAVE_DIR".backups
  $RUN $BIN --import-save $F || rc=1
  cmp -s $KEEP/BISLPM-65495MH/BISLPM-65495MH "$MH1_SAVE_DIR/BISLPM-65495MH/BISLPM-65495MH" || { echo "$e: data file differs"; rc=1; }
  RT_QUEST_TRACE=1 $RUN $BIN disc/mh1 --boot --input "$(cat tools/pc_scripts/continue.txt)" --shot $OUT/imp_$e.png --time 60 \
      2> $OUT/imp_$e.log >/dev/null
  if grep -q "money 1550" $OUT/imp_$e.log; then echo "$e: import + CONTINUE shows 1550z"; else echo "$e: CONTINUE FAILED (see $OUT/imp_$e.log)"; rc=1; fi
done
# the exe given a dropped save (as Windows does): imports, then starts the game
rm -rf "$MH1_SAVE_DIR"
RT_QUEST_TRACE=1 $RUN $BIN disc/mh1 $OUT/exp.psu --frames 3 >/dev/null 2>&1
[ -f "$MH1_SAVE_DIR/BISLPM-65495MH/BISLPM-65495MH" ] && echo "drop on exe: imported" || { echo "drop on exe: FAILED"; rc=1; }
[ $rc = 0 ] && echo "save import OK" || echo "save import FAILED"
exit $rc
