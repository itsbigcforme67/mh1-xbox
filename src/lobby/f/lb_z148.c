/* lb_z148 - auto-drafted 0x005ED350-0x005ED380: inflate_trees_fixed (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s32 fixed_bd;
extern s32 fixed_bl;
extern char fixed_tl[];
extern char fixed_td[];

s32 inflate_trees_fixed(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (*(int *)arg0) = fixed_bl;
    (*(int *)arg1) = fixed_bd;
    (*(int *)arg2) = (int)&fixed_tl;
    (*(int *)arg3) = (int)&fixed_td;
    return 0;
}
