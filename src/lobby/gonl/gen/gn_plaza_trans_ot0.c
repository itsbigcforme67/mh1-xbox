/* G online (PC): plaza_trans_ot0, written from Monster Hunter G's code at lobby 0x578D70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 DispButtonHelp();
s32 DispHelpLine();
s32 DispSceneSubTitle();
s32 DispSceneTitle();
s32 Plaza_disp_ReibunEdit();
s32 lobby_func_00574D70();
s32 lobby_func_00574FC0();
s32 lobby_func_00576300();
s32 lobby_func_005774D0();
s32 lobby_func_005782A0();
s32 lobby_func_00578C40();
s32 lobby_func_0059F6E0();
s32 plaza_checkMyStatusTrans();
s32 plaza_enterLobbyTrans();
s32 plaza_mailBoxTrans();
s32 plaza_movePlazaTrans();
s32 plaza_setMyCommentTrans();
extern u8 lb_shop_msg;
extern u8 pNet;
extern u8 textLobbyTbl;
extern u8 tl_mail_tbl;
s32 DispFrameMessage();          /* extern */
s32 Draw_square(); /* extern */
s32 Plaza_disp_chat();                          /* extern */
s32 Plaza_disp_chatlog();                       /* extern */
s32 Put_2TF();                         /* extern */
s32 SetFilterMode();                     /* extern */
s32 SetTextureStage();                   /* extern */
s32 flSetRenderState();         /* extern */
s32 flfntLocate();              /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print();           /* extern */
s32 font_set_palette();                  /* extern */
s32 font_set_stack_no();                     /* extern */
s32 han2zen();              /* extern */
s32 reload_tex();               /* extern */
s32 sprintf(); /* extern */
extern u8 ClassInfo;
extern s32 g_main_30645A;
extern u8 lit_2316;
extern s128 lit_3199;
extern u8 lit_3241;

void plaza_trans_ot0(void *arg0) {
    u8 g_frame[0x90] __attribute__((aligned(16)));   /* G's stack frame */
#define sp20 (*(s128 *)(g_frame + 0x20))
#define sp2C (*(s32 *)(g_frame + 0x2C))
#define sp30 (*(s32 *)(g_frame + 0x30))
#define sp70 (*(s32 *)(g_frame + 0x70))
    s32 var_s0;
    u8 temp_v0;
    u8 temp_v1;
    u8 temp_v1_2;

    var_s0 = 0xFF537F56;
    sp20 = lit_3199;
    font_set_stack_no((*(s32 *)((u8 *)(arg0) + 0x18)));
    reload_tex(1, 0x180);
    SetTextureStage(0x180);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(&(*(u8 *)((u8 *)&textLobbyTbl + 0x14)));
    lobby_func_00578C40((s32) (((s16) (((*(s16 *)((u8 *)&textLobbyTbl + 0x16)) + 0x1B)))));
    DispSceneTitle();
    DispHelpLine();
    temp_v1 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28));
    switch (temp_v1) {                              /* switch 1; irregular */
    case 11:                                        /* switch 1 */
        var_s0 = 0xFF564D33;
        sp2C = 0xC6201A14;
        lobby_func_0059F6E0(-0x39DFE5EC);
        break;
    case 16:                                        /* switch 1 */
    case 12:                                        /* switch 1 */
        var_s0 = 0xFF564D33;
        sp2C = 0xCC151200;
        lobby_func_0059F6E0(-0x33EAEE00);
        break;
    }
    temp_v1_2 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28));
    if ((temp_v1_2 != 0xE) && (temp_v1_2 != 4) && (temp_v1_2 != 3) && (temp_v1_2 != 8) && (temp_v1_2 != 0)) {
        DispFrameMessage(&sp20, 0);
        Draw_square(0xD8, 0x76, 0x192, 1, var_s0);
        Draw_square(0xD8, 0x13C, 0x192, 1, var_s0);
    }
    temp_v0 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28));
    switch (temp_v0) {                              /* switch 2 */
    default:                                        /* switch 2 */
        lobby_func_0059F6E0(-0x33EAEE00);
        Plaza_disp_chat();
        break;
    case 0:                                         /* switch 2 */
        plaza_enterLobbyTrans(0xD8, 0x50);
        break;
    case 1:                                         /* switch 2 */
        plaza_movePlazaTrans();
        break;
    case 3:                                         /* switch 2 */
        lobby_func_00574FC0(0xD8, 0x50, 0);
        break;
    case 14:                                        /* switch 2 */
        lobby_func_00574D70(0xD8, 0x50, 0);
        break;
    case 5:                                         /* switch 2 */
        lobby_func_00576300();
        break;
    case 6:                                         /* switch 2 */
        lobby_func_005774D0();
        break;
    case 4:                                         /* switch 2 */
        plaza_mailBoxTrans(0xD8, 0x50, 0);
        break;
    case 7:                                         /* switch 2 */
        plaza_checkMyStatusTrans();
        break;
    case 8:                                         /* switch 2 */
        plaza_setMyCommentTrans(0xD8, 0x50, 0);
        break;
    case 9:                                         /* switch 2 */
        lobby_func_005782A0();
        break;
    case 10:                                        /* switch 2 */
        Plaza_disp_ReibunEdit();
        break;
    case 11:                                        /* switch 2 */
        Plaza_disp_chatlog();
        break;
    }
    DispSceneSubTitle();
    DispButtonHelp((*(void **)&pNet));
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    sprintf(&sp70, &lit_3241, (*(s32 *)((u8 *)&tl_mail_tbl + 0x30)), (*(s32 *)((u8 *)&g_main_30645A + (ClassInfo * 0x15C))), (*(s32 *)((u8 *)&lb_shop_msg + 0x8)));
    han2zen(&sp70, &sp30);
    flfntLocate(0x1AE, 0x1F);
    font_print(&lit_2316, &sp30);
}

