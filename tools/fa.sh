#!/bin/bash
# usage: fa.sh funcname  -> asm without address columns
cd "$(dirname "$0")/.."
f=$(grep -l "^glabel $1\$" asm/game/text/*.s | head -1)
awk "/^glabel $1\$/,/^endlabel $1\$/" $f | sed 's#/\* [0-9A-F]* [0-9A-F]* [0-9A-F]* \*/##'
