#!/bin/bash
# usage: cnt.sh file func  -> prints check.py line
python3 tools/check.py "$1" | grep " $2 "
