#!/bin/sh
# symsz.sh NAME...: print address/size lines of NAME from config/symbols/main.txt
cd "$(dirname "$0")/.." || exit 1
for n in "$@"; do grep -h "^$n = " config/symbols/main.txt; done
