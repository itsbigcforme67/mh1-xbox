#!/bin/bash
# xb.sh tap <button> [ms] | shot <name> | hold <hex> <ms>
T="$(cd "$(dirname "$0")" && pwd)"; S="$T/../../build/bench"
hub() { printf '%s\n' "$1" | timeout 3 nc -q0 127.0.0.1 24851 >/dev/null; }
declare -A B=([a]=0001 [b]=0002 [x]=0004 [y]=0008 [lb]=0010 [rb]=0020 [back]=0040 [start]=0080 [up]=0800 [down]=1000 [left]=2000 [right]=4000)
case $1 in
tap) hub "pad ${B[$2]}"; sleep $(echo "${3:-120}/1000" | bc -l); hub "pad 0" ;;
hold) hub "pad $2"; sleep $(echo "$3/1000" | bc -l); hub "pad 0" ;;
shot) timeout 20 ffmpeg -hide_banner -loglevel fatal -f v4l2 -input_format mjpeg -video_size 1920x1080 -i /dev/video2 -t 4 -update 1 -vf "crop=1440:1080:240:0,scale=960:720" -y $S/$2.png && echo $S/$2.png ;;
esac
