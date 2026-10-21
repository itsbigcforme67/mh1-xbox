/* G online (PC): lobby_func_0056C3F0, written from Monster Hunter G's code at lobby 0x56C3F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 str_fadein_vol();
s32 str_pause();
extern u8 D_0035EFEB;
extern u8 Snd_bgm_tbl;
extern u8 cw;
extern u8 game_w;
extern u8 lb_sys;
extern u8 pNet;

void lobby_func_0056C3F0(void) {
    str_pause(0, 0);
    str_pause(1, 0);
    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
        str_fadein_vol(0, 0x1E, (s32) (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&Snd_bgm_tbl + 0x1)) + ((*(u8 *)((u8 *)&game_w + 0x14)) * 2))));
        (*(s8 *)((u8 *)&lb_sys + 0x94)) = 1;
    }
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
    (*(s8 *)&D_0035EFEB) = 1;
}

