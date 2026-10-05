#!/bin/sh
# Regenerate the matching-run files of the lobby cnet near-match file and (re)register them in c_files.txt.
cd "$(dirname "$0")/.." || exit 1
python3 tools/lbfieldcheck.py || exit 1
rm -f src/lobby/cnet/cnlbs.c src/lobby/cnet/cnlbs[b-z].c src/lobby/cnet/cnlbs[0-9]*.c
python3 tools/lbruns.py src/lobby/cnet/cnlbs_nm.c src/lobby/cnet/cnlbs cnet/cnlbs > /tmp/lbruns.txt 2>/tmp/lbruns.err || { cat /tmp/lbruns.err; exit 1; }
grep -v ' cnet/cnlbs' config/c_files.txt > /tmp/c_files.new
grep -v '^$' /tmp/lbruns.txt >> /tmp/c_files.new
cp /tmp/c_files.new config/c_files.txt
echo "registered $(grep -c ' cnet/cnlbs' config/c_files.txt) runs"
