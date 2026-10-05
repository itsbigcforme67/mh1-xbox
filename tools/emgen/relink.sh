#!/bin/bash
# relink.sh PREFIX NM COMMENT...  (e.g. em14_r src/game/em/em14_nm.c "monster 14 AI")
cd "/home/james/claude projects/MH XBOX/mh1-wt/D"
pre=$1; nm=$2; cmt=$3
grep -v "em/${pre}[0-9][0-9]\$" config/c_files.txt > /tmp/claude-1000/w/cf_tmp.txt && cp /tmp/claude-1000/w/cf_tmp.txt config/c_files.txt
rm -f src/game/em/${pre}[0-9][0-9].c
/tmp/claude-1000/w/runall.sh game $nm src/game/em $pre "$cmt" 2>&1 | grep -E "iter|# verify"
grep -E "^game" /tmp/claude-1000/w/mk.txt | sed 's/ *#.*//' >> config/c_files.txt
