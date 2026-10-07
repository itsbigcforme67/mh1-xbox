#!/bin/bash
cd "$(cd "$(dirname "$0")/../.." && pwd)"
tools/compilers/wibo tools/compilers/mwcps2-3.0b52-030722/mwccps2.exe -c -O4,p -nostdinc -stderr -Iinclude -pragma "divbyzerocheck on" "$1" -o ${2:-$B_SCRATCH$/x.o} 2>&1
