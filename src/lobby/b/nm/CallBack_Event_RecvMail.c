#include "lobby_b.h"
extern char RecvMailInfo[];
extern char D_39F5EC[];
extern char D_39F686[];
extern char D_39F552[];
extern char D_39F5EC[];
extern char D_39F4B8[];
extern char D_39F552[];
extern char D_39F41E[];
extern char D_39F4B8[];
extern char D_39F384[];
extern char D_39F41E[];
extern char D_39F2EA[];
extern char D_39F384[];
extern char RecvMailInfo[];
extern char D_39F2EA[];
extern char D_39F251[];
extern char D_39F259[];
extern char D_39F26A[];
extern char D_39F26A[];
void CallBack_Event_RecvMail(CNET_RES res) {
    char sp3C[0x80];
    char sp28[0x10];
    char sp20[0x9C];
    int var_a1_2;
    int var_a1_3;
    int var_a1_4;
    int var_a1_5;
    int var_a1_6;
    int var_a1_7;
    int var_a1_8;
    int var_a2;
    int var_a2_2;
    int var_a2_3;
    int var_a2_4;
    int var_a2_5;
    int var_a2_6;
    s32 var_a0;
    s32 var_a0_2;
    s32 var_a0_3;
    s32 var_a0_4;
    s32 var_a0_5;
    s32 var_a0_6;
    s32 var_a0_7;
    s32 var_a0_8;
    s32 var_a1;
    s8 temp_v0;
    s8 temp_v0_2;
    s8 temp_v0_3;
    s8 temp_v0_4;
    s8 temp_v0_5;
    s8 temp_v0_6;
    s8 temp_v0_7;
    int var_a2_7;
    int var_v1;
    int temp_a0;
    int temp_v1;

    var_a0 = 0;
    var_a1 = 0;
    var_v1 = (int)&RecvMailInfo;
    do {
        if ((*(u8 *)var_v1) != 0) {
            var_a0 += 1;
        }
        var_a1 += 1;
        var_v1 += 0x9A;
    } while (var_a1 < 8);
    if (var_a0 == 0) {
        cnWrap_SoundRequest(5, var_a1);
    }
    memset(sp20, 0, 0x9C);
    cnLBS_Get_RecvMessage(sp20, sp28, sp3C);
    var_a2 = (int)&D_39F5EC;
    var_a1_2 = (int)&D_39F686;
    var_a0_2 = 0x4D;
    do {
        var_a0_2 -= 1;
        temp_v0 = F(s8, var_a2, 1);
        F(s8, var_a1_2, 0) = (s8) F(s8, var_a2, 0);
        var_a2 += 2;
        F(s8, var_a1_2, 1) = temp_v0;
        var_a1_2 += 2;
    } while (var_a0_2 > 0);
    var_a2_2 = (int)&D_39F552;
    var_a1_3 = (int)&D_39F5EC;
    var_a0_3 = 0x4D;
    do {
        var_a0_3 -= 1;
        temp_v0_2 = F(s8, var_a2_2, 1);
        F(s8, var_a1_3, 0) = (s8) F(s8, var_a2_2, 0);
        var_a2_2 += 2;
        F(s8, var_a1_3, 1) = temp_v0_2;
        var_a1_3 += 2;
    } while (var_a0_3 > 0);
    var_a2_3 = (int)&D_39F4B8;
    var_a1_4 = (int)&D_39F552;
    var_a0_4 = 0x4D;
    do {
        var_a0_4 -= 1;
        temp_v0_3 = F(s8, var_a2_3, 1);
        F(s8, var_a1_4, 0) = (s8) F(s8, var_a2_3, 0);
        var_a2_3 += 2;
        F(s8, var_a1_4, 1) = temp_v0_3;
        var_a1_4 += 2;
    } while (var_a0_4 > 0);
    var_a2_4 = (int)&D_39F41E;
    var_a1_5 = (int)&D_39F4B8;
    var_a0_5 = 0x4D;
    do {
        var_a0_5 -= 1;
        temp_v0_4 = F(s8, var_a2_4, 1);
        F(s8, var_a1_5, 0) = (s8) F(s8, var_a2_4, 0);
        var_a2_4 += 2;
        F(s8, var_a1_5, 1) = temp_v0_4;
        var_a1_5 += 2;
    } while (var_a0_5 > 0);
    var_a2_5 = (int)&D_39F384;
    var_a1_6 = (int)&D_39F41E;
    var_a0_6 = 0x4D;
    do {
        var_a0_6 -= 1;
        temp_v0_5 = F(s8, var_a2_5, 1);
        F(s8, var_a1_6, 0) = (s8) F(s8, var_a2_5, 0);
        var_a2_5 += 2;
        F(s8, var_a1_6, 1) = temp_v0_5;
        var_a1_6 += 2;
    } while (var_a0_6 > 0);
    var_a2_6 = (int)&D_39F2EA;
    var_a1_7 = (int)&D_39F384;
    var_a0_7 = 0x4D;
    do {
        var_a0_7 -= 1;
        temp_v0_6 = F(s8, var_a2_6, 1);
        F(s8, var_a1_7, 0) = (s8) F(s8, var_a2_6, 0);
        var_a2_6 += 2;
        F(s8, var_a1_7, 1) = temp_v0_6;
        var_a1_7 += 2;
    } while (var_a0_7 > 0);
    var_a2_7 = (int)&RecvMailInfo;
    var_a1_8 = (int)&D_39F2EA;
    var_a0_8 = 0x4D;
    do {
        var_a0_8 -= 1;
        temp_v0_7 = F(s8, var_a2_7, 1);
        F(s8, var_a1_8, 0) = (s8) F(u8, var_a2_7, 0);
        var_a2_7 += 2;
        F(s8, var_a1_8, 1) = temp_v0_7;
        var_a1_8 += 2;
    } while (var_a0_8 > 0);
    strncpy(&D_39F251, sp20, 8);
    strncpy(&D_39F259, sp28, 0x10);
    strncpy(&D_39F26A, sp3C, 0x80);
    KinshiYogo_chk(&D_39F26A);
    temp_a0 = (int)cw;
    *(s8 *)0x39F250 = 1;
    F(u8, temp_a0, 0x2F7E) = (u8) (F(u8, temp_a0, 0x2F7E) + 1);
    temp_v1 = (int)cw;
    if (F(u8, temp_v1, 0x2F7E) >= 9) {
        F(u8, temp_v1, 0x2F7E) = 8U;
    }
}
