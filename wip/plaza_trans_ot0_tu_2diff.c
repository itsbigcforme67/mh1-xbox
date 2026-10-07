extern u8 D_3A27D6[];
extern char lit_3241[];
typedef struct { u8 p[0xF0]; s16 x; s16 y; } TXF;
void plaza_trans_ot0(a)
LB_NETW *a;
{
    int sp60[3] = {0, 0x01C00280, 0xB0101010};
    u32 col = 0xFF602020;
    char sp40[0x20];
    char sp30[0x10];
    int sp20[4] = {0x005600D8, 0x0D1B120F, 0x80000000, 0xFF2A0000};
    TXF *t;
    u8 *p;

    font_set_stack_no(*(int *)((u8 *)a + 0x18));
    if (pNet->depth > 1) {
        Put_F(sp60);
    }
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(textLobbyTbl + 0x14);
    DispSceneTitle();
    DispHelpLine();
    p = &pNet->sel;
    if (pNet->sel == 0xE || pNet->sel == 0xC) {
        col = 0xFF606025;
        sp20[3] = 0xCC151200;
    }
    switch (*p) {
    case 0:
    case 8:
    case 3:
    case 4:
        break;
    default:
        DispFrameMessage(sp20, 0);
        Draw_square(0xD8, 0x76, 0x192, 1, col);
        Draw_square(0xD8, 0x13C, 0x192, 1, col);
        break;
    }
    switch (pNet->sel) {
    case 12:
    case 14:
    default:
        SetSceneSubTitleColor(0xCC151200);
        plaza_chatTrans(pNet);
        Plaza_disp_chat();
        break;
    case 0:
        plaza_enterLobbyTrans(0xD8, 0x50);
        break;
    case 1:
        plaza_movePlazaTrans();
        break;
    case 2:
        break;
    case 3:
        plaza_checkFriendTrans(0xD8, 0x50, 0);
        break;
    case 5:
    case 6:
        plaza_searchMemberTrans();
        break;
    case 4:
        plaza_mailBoxTrans(0xD8, 0x50, 0);
        break;
    case 7:
        plaza_checkMyStatusTrans();
        break;
    case 8:
        plaza_setMyCommentTrans(0xD8, 0x50, 0);
        break;
    case 9:
        DispFrameMessage(sp20, 0);
        Draw_square(0xD8, 0x76, 0x192, 1, col);
        Draw_square(0xD8, 0x13C, 0x192, 1, col);
        plaza_setChatModeTrans();
        plaza_chatTrans(pNet);
        break;
    case 10:
        Plaza_disp_ReibunEdit();
        break;
    case 11:
        Plaza_disp_chatlog();
        break;
    case 13:
        break;
    }
    DispSceneSubTitle();
    DispButtonHelp(pNet);
    flfntSetSize(0x12, 0x12);
    t = (TXF *)text_lobby_msg[2];
    Lb_put_msg_type2(&t->x);
    sprintf(sp40, lit_3241, *(u16 *)(D_3A27D6 + ClassInfo.plaza * 0x15C));
    han2zen(sp40, sp30);
    font_set_palette(0);
    flfntLocate(t->x + 0x48, t->y);
    font_print(lit_2316, sp30);
}
