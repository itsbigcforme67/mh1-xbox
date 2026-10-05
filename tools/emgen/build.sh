#!/bin/bash
# build.sh NN FILE   (FILE e.g. f_em_5D9EE0)
NN=$1; F=$2
cd "/home/james/claude projects/MH XBOX/mh1-wt/D"
A=/tmp/claude-1000/w/asm/$F.s
O=wip/em${NN}_nm.c
python3 /tmp/claude-1000/w/fres.py $A /tmp/claude-1000/w/$F.d2.c /tmp/claude-1000/w/$F.d3.c >/dev/null
python3 /tmp/claude-1000/w/d2c2.py /tmp/claude-1000/w/$F.d3.c $O $NN "em$NN draft"
python3 /tmp/claude-1000/w/pipe.py $O
python3 /tmp/claude-1000/w/bitfix.py $O $A
python3 /tmp/claude-1000/w/postgen.py $NN $O
python3 /tmp/claude-1000/w/postgen2.py $NN $O
[ -f /tmp/claude-1000/w/extra$NN.sh ] && bash /tmp/claude-1000/w/extra$NN.sh
python3 /tmp/claude-1000/w/postgen2.py $NN $O >/dev/null
