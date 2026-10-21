/* G online (PC): lobby_func_005936B0, written from Monster Hunter G's code at lobby 0x5936B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Put_2TF();
s32 SetFilterMode();
s32 SetTextureStage();
s32 flSetRenderState();
s32 reload_tex();
extern u8 lb_button_tbl;

void lobby_func_005936B0(void) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s32 *)(g_frame + 0x10))

    (*(s128 *)((u8 *)(&sp10) + 0)) = (s128) (*(s128 *)((u8 *)(&(*(u8 *)((u8 *)&lb_button_tbl + 0x80))) + 0));
    (*(f32 *)((u8 *)(&sp10) + 0x10)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_button_tbl + 0x80))) + 0x10));
    SetFilterMode(1);
    flSetRenderState(0x60, 0, M2C_ERROR(/* Read from unset register $a2 */));
    reload_tex(1, 0x17C);
    SetTextureStage(0x17C);
    Put_2TF(&sp10);
}

