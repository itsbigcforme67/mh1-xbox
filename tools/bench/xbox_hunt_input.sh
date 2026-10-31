#!/bin/bash
# Hunt inputs: the room script of tools/test_online_g.sh HUNT=1, Xbox in ANNA's place, PC in BOB's.
HUNT=1
IN="idle*200$(for i in $(seq 14); do printf ',circle*2,idle*150'; done)$(for i in $(seq 12); do printf ',circle*2,idle*120,dleft*2,idle*30,circle*2,idle*150'; done),idle*${HOLD:-900}${TAIL:+,$TAIL}"
    BASE=$(python3 -c "
b='$IN'.split(',')
o=[];f=0
for it in b:
    k,c=it.split('*');c=int(c)
    if f+c>2620:
        o.append('idle*%d'%(2620-f)) if k=='idle' and 2620>f else None
        break
    o.append(it);f+=c
print(','.join(o))")
    rep() { i=0; while [ $i -lt $1 ]; do printf "%s" "$2"; i=$((i + 1)); done; }
    CARDK=""; [ -n "$HUNT" ] && CARDK=$(rep ${CARD:-3} ',dright*2,idle*48')
    IA="$BASE,square*2,idle*270$(rep 4 ',circle*2,idle*148')$CARDK$(rep 5 ',circle*2,idle*148')"
    for k in circle circle dleft dleft ddown ddown ddown circle circle circle; do IA="$IA,$k*2,idle*100"; done
    END=$(rep 36 ',circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,circle*2,idle*13,cam_u*2,idle*13,cross*2,idle*18,ddown*2,idle*18,circle*2,idle*13,cam_u*2,idle*13')
    END="$END$(rep 60 ',circle*2,idle*91')"
    IA="$IA,idle*2200$(rep 4 ',square*2,idle*120,circle*2,idle*120,circle*2,idle*160')$END"
    IB="$BASE,square*2,idle*2678,square*2,idle*60$(rep 20 ',circle*2,idle*58'),idle*300$(rep 3 ',square*2,idle*100,circle*2,idle*100')$END"
printf "%s" "$IA" > "build/bench/online/ia.txt"
printf "%s" "$IB" > "build/bench/online/ib.txt"
