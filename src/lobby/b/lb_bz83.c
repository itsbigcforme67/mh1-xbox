/* lb_bz83 - lobby UI/client 0x005B79B0-0x005B7B34: text_lobby_trans_ot0 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char CnetWork[];
extern char textLobbyTbl[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x1]; u8 x2C33; } CWS_text_lobby_trans_ot0;
typedef struct { u8 pad0000[0x18]; s32 x0018; } ARG_text_lobby_trans_ot0_arg0;

void text_lobby_trans_ot0(ARG_text_lobby_trans_ot0_arg0 *arg0) {
    u8 temp_a0;
    u8 temp_v1;

    font_set_stack_no(arg0->x0018);
    if (Online_ck() == 1) {
        temp_v1 = ((CWS_text_lobby_trans_ot0 *)cw)->x2C31;
        if ((temp_v1 == 0) && (((CWS_text_lobby_trans_ot0 *)cw)->x2C33 < 7) && (F(u8, &CnetWork, 5) == 0)) {
            reload_tex(1, 0x14D);
            SetTextureStage(0x14D);
            SetFilterMode(1);
            flSetRenderState(0x60, 0);
            Put_2TF((u8 *)&textLobbyTbl + 0x28);
        } else if (((u32) (temp_v1 - 5) <= 1U) || (temp_v1 == 3)) {
            Disp_back();
        } else {
            reload_tex(1, 0x154);
            SetTextureStage(0x154);
            SetFilterMode(1);
            flSetRenderState(0x60, 0);
            Put_2TF(&textLobbyTbl);
        }
    } else {
        reload_tex(1, 0x119);
        SetTextureStage(0x119);
        Put_2TF((u8 *)&textLobbyTbl + 0x28);
    }
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    if (((CWS_text_lobby_trans_ot0 *)cw)->x2C31 != 5) {
        temp_a0 = ((CWS_text_lobby_trans_ot0 *)cw)->x2C33;
        if ((temp_a0 == 4) && (temp_a0 == 3)) {
            DispSceneTitle(temp_a0, ((CWS_text_lobby_trans_ot0 *)cw));
            DispHelpLine();
        }
    }
}
