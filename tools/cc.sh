#!/bin/bash
# cc.sh file.c : compile with the project flags and disassemble (for experiments)
cd "$(dirname "$0")/.."
out=/tmp/cc_$$.o
tools/compilers/wibo tools/compilers/mwcps2-3.0b52-030722/mwccps2.exe -c -O4,p -nostdinc -stderr -Iinclude -pragma "divbyzerocheck on" "$1" -o $out >/dev/null 2>&1 || tools/compilers/wibo tools/compilers/mwcps2-3.0b52-030722/mwccps2.exe -c -O4,p -nostdinc -stderr -Iinclude -pragma "divbyzerocheck on" "$1" -o $out
tools/binutils/bin/mips64r5900el-ps2-elf-objdump -dr --no-show-raw-insn $out | sed 's/^ *[0-9a-f]*:\t//' 
rm -f $out
