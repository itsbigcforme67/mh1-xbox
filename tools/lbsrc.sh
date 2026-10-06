#!/bin/sh
# lbsrc.sh NAME... : print the auto-converted source (build/lbauto/NAME.c or .err.c) without repeated extern lines
for n in "$@"; do
  echo "=========== $n"
  f=build/lbauto/$n.c; [ -f $f ] || f=build/lbauto/$n.err.c
  if [ -f $f ]; then sed -n '2,$p' $f | grep -v '^$' | awk '!seen[$0]++ || !/^extern/'; else echo "(no auto source; see the m2c draft)"; fi
done
