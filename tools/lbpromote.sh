#!/bin/sh
# lbpromote.sh NAME... : promote matching single-function near-match files src/lobby/b/nm/NAME.c into linked runs
# src/lobby/b/lb_byNN.c (via tools/lbf_merge.py), register them in config/c_files.txt and delete the nm drafts.
cd "$(dirname "$0")/.." || exit 1
FL=$(mktemp); mkdir -p src/lobby/_one; good=""
for n in "$@"; do
    f=src/lobby/b/nm/$n.c
    line=$(python3 tools/check.py $f 2>&1 | grep "^OK  $n ")
    [ -z "$line" ] && { echo "not matching: $n"; continue; }
    echo "$line" | awk '{print $4, $2, $5}' >> $FL
    cp $f src/lobby/_one/$n.c; good="$good $n"
done
[ -z "$good" ] && exit 1
LBFL=$FL LBDIR=b python3 tools/lbf_merge.py lb_by "agent B promoted near-match" $good
for n in $good; do
    if grep -q "b/lb_by[0-9]*" config/c_files.txt && grep -rqw "$n" src/lobby/b/lb_by*.c; then rm -f src/lobby/b/nm/$n.c; fi
    rm -f src/lobby/_one/$n.c
done
rmdir src/lobby/_one 2>/dev/null
