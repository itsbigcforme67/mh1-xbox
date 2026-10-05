#!/bin/sh
# kill permuter runs for one function name given as $1
for pid in $(ps -eo pid,args | awk -v f="$1" '$0 ~ ("perm.py game " f " ") || $0 ~ ("perm/" f "( |$)") {print $1}'); do
  [ "$pid" != "$$" ] && kill "$pid" 2>/dev/null
done
