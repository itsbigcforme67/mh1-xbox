#!/bin/bash
# usage: runall.sh MODULE NM OUTDIR PREFIX COMMENT  -> iterates mkruns3 --verify until stable; prints final lines
cd "/home/james/claude projects/MH XBOX/mh1-wt/D"
mod=$1; nm=$2; out=$3; pre=$4; cmt=$5
skip=""
for it in 1 2 3 4 5 6; do
  rm -f $out/${pre}[0-9][0-9].c
  if [ -z "$skip" ]; then python3 tools/mkruns3.py $mod $nm $out $pre 1 "$cmt" --verify > /tmp/claude-1000/w/mk.txt 2>&1
  else python3 tools/mkruns3.py $mod $nm $out $pre 1 "$cmt" --skip $skip --verify > /tmp/claude-1000/w/mk.txt 2>&1; fi
  bad=$(grep "^# verify" /tmp/claude-1000/w/mk.txt | sed 's/.*run: //')
  echo "iter $it bad: $bad"
  if [ "$bad" = "none" ]; then break; fi
  skip="$skip,$(echo $bad | tr -d ' ')"
  skip=$(echo $skip | sed 's/^,//')
done
cat /tmp/claude-1000/w/mk.txt
