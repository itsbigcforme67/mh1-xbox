/* G online (PC): lbc_game_ready_00, written from Monster Hunter G's code at lobby 0x583B00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_Match_MatchInformation();
s32 Check_CallBackWait();
s32 cnLBS_Get_MatchInfomation();
s32 cnLBS_Read_MatchInfomation();
void * memcpy();
void * memset();
extern u8 USER_PL_ID;
extern u8 cw;
extern s32 g_sub_541E30;
extern s32 room_member_handle;
extern s32 room_member_id;
extern s32 room_member_mini_data;

void lbc_game_ready_00(s32 unused_a0, s32 unused_a1, s32 unused_a2, s32 arg3) {
    u8 g_frame[0x780] __attribute__((aligned(16)));   /* G's stack frame */
#define sp70 (*(u8 *)(g_frame + 0x70))
#define sp71 (*(u8 *)(g_frame + 0x71))
#define sp72 (*(s32 *)(g_frame + 0x72))
    u8 *var_s2;   /* (s32 *) */
    u8 *var_s3;   /* (s32 *) */
    u8 *var_s4;   /* (s32 *) */
    s32 var_s0;
    s32 var_s1;
    u8 *var_s5;
    u8 temp_a0;
    u8 *temp_a1;
    u8 *temp_v1;
    u8 *temp_v1_2;

    temp_a1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xB9C));
    switch (temp_a0) {                              /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_a1) + 0xB9C)) = (u8) (temp_a0 + 1);
        /* fallthrough */
    case 1:
        temp_v1 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xB9C)) + 1);
        /* fallthrough */
    case 2:
        temp_v1_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x30;
        cnLBS_Read_MatchInfomation((s32) CallBack_Result_Match_MatchInformation);
        return;
    case 3:
        Check_CallBackWait((s32) temp_a0, (s32) temp_a1, (s32) (temp_a1 + 0xB9C), arg3);
        return;
    case 4:
        (*(u8 *)((u8 *)(temp_a1) + 0xB9B)) = (u8) ((*(u8 *)((u8 *)(temp_a1) + 0xB9B)) + 1);
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0U;
        cnLBS_Get_MatchInfomation(&sp70);
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB1)) = sp70;
        (*(u8 *)&USER_PL_ID) = sp71;
        memcpy((*(void **)&cw) + 0xFA6C, &sp72, 0x10);
        memcpy(&g_sub_541E30, (*(void **)&cw) + 0xFA6C, 0xF);
        var_s1 = 0;
        if ((s32) (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB1)) > 0) {
            var_s0 = 0;
            var_s5 = &sp70;
            var_s4 = &room_member_id;
            var_s3 = &room_member_handle;
            var_s2 = &room_member_mini_data;
            do {
                memset((*(void **)&cw) + var_s0 + 0x21C, 0, 0x5C);
                memcpy((*(void **)&cw) + var_s0 + 0x21C, var_s5 + 0x114, 8);
                memcpy((*(void **)&cw) + var_s0 + 0x224, var_s5 + 0x11C, 0x10);
                memcpy((*(void **)&cw) + var_s0 + 0x238, var_s5 + 0x130, 0x40);
                memset(var_s4, 0, 8);
                memcpy(var_s4, var_s5 + 0x114, 8);
                memcpy(var_s3, var_s5 + 0x11C, 0x11);
                memcpy(var_s2, var_s5 + 0x130, 0x40);
                var_s1 += 1;
                var_s0 += 0x5C;
                var_s5 += 0x98;
                var_s4 += 8;
                var_s3 += 0x11;
                var_s2 += 0x40;
            } while (var_s1 < (s32) (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB1)));
        }
        return;
    }
}

