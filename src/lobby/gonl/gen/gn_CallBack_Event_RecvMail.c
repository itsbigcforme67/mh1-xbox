/* G online (PC): CallBack_Event_RecvMail, written from Monster Hunter G's code at lobby 0x586DF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 KinshiYogo_chk();
s32 cnWrap_SoundRequest();
s32 memcmp();
void * memset();
void * strncpy();
extern u8 g_main_2EFBAC;
extern u8 g_main_2EFBAC;
s32 cnLBS_Get_RecvMessage(); /* extern */
extern s32 g_main_302290;
extern u8 RecvMailInfo;

void CallBack_Event_RecvMail(void) {
    u8 g_frame[0xE0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp40 (*(s32 *)(g_frame + 0x40))
#define sp48 (*(s32 *)(g_frame + 0x48))
#define sp5C (*(s32 *)(g_frame + 0x5C))
    u8 *var_a1_2;   /* (s32 *) */
    u8 *var_a1_3;   /* (s32 *) */
    u8 *var_a1_4;   /* (s32 *) */
    u8 *var_a1_5;   /* (s32 *) */
    u8 *var_a1_6;   /* (s32 *) */
    u8 *var_a1_7;   /* (s32 *) */
    u8 *var_a1_8;   /* (s32 *) */
    u8 *var_a2;   /* (s32 *) */
    u8 *var_a2_2;   /* (s32 *) */
    u8 *var_a2_3;   /* (s32 *) */
    u8 *var_a2_4;   /* (s32 *) */
    u8 *var_a2_5;   /* (s32 *) */
    u8 *var_a2_6;   /* (s32 *) */
    u8 *var_s0;   /* (s32 *) */
    s32 var_a0_2;
    s32 var_a0_3;
    s32 var_a0_4;
    s32 var_a0_5;
    s32 var_a0_6;
    s32 var_a0_7;
    s32 var_a0_8;
    s32 var_a1;
    s32 var_s1;
    s32 var_v1;
    s8 temp_v0;
    s8 temp_v0_2;
    s8 temp_v0_3;
    s8 temp_v0_4;
    s8 temp_v0_5;
    s8 temp_v0_6;
    s8 temp_v0_7;
    u8 *var_a0;
    u8 *var_a2_7;
    u8 *temp_a0;
    u8 *temp_v1;

    memset(&sp40, 0, 0x9C);
    cnLBS_Get_RecvMessage(&sp40, &sp48, &sp5C);
    var_s1 = 0;
    var_s0 = &g_main_302290;
loop_1:
    if (memcmp(var_s0, &sp40, 8) != 0) {
        var_s1 += 1;
        var_s0 += 0x18;
        if (var_s1 >= 8) {
            var_a1 = 0;
            var_v1 = 0;
            var_a0 = &RecvMailInfo;
            do {
                if (*var_a0 != 0) {
                    var_a1 += 1;
                }
                var_v1 += 1;
                var_a0 += 0x9A;
            } while (var_v1 < 8);
            if (var_a1 == 0) {
                cnWrap_SoundRequest(5);
            }
            var_a2 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x39C));
            var_a1_2 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x436));
            var_a0_2 = 0x4D;
            do {
                var_a0_2 -= 1;
                temp_v0 = (*(s8 *)((u8 *)(var_a2) + 1));
                (*(s8 *)((u8 *)(var_a1_2) + 0)) = (s8) (*(s8 *)((u8 *)(var_a2) + 0));
                var_a2 += 2;
                (*(s8 *)((u8 *)(var_a1_2) + 1)) = temp_v0;
                var_a1_2 += 2;
            } while (var_a0_2 > 0);
            var_a2_2 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x302));
            var_a1_3 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x39C));
            var_a0_3 = 0x4D;
            do {
                var_a0_3 -= 1;
                temp_v0_2 = (*(s8 *)((u8 *)(var_a2_2) + 1));
                (*(s8 *)((u8 *)(var_a1_3) + 0)) = (s8) (*(s8 *)((u8 *)(var_a2_2) + 0));
                var_a2_2 += 2;
                (*(s8 *)((u8 *)(var_a1_3) + 1)) = temp_v0_2;
                var_a1_3 += 2;
            } while (var_a0_3 > 0);
            var_a2_3 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x268));
            var_a1_4 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x302));
            var_a0_4 = 0x4D;
            do {
                var_a0_4 -= 1;
                temp_v0_3 = (*(s8 *)((u8 *)(var_a2_3) + 1));
                (*(s8 *)((u8 *)(var_a1_4) + 0)) = (s8) (*(s8 *)((u8 *)(var_a2_3) + 0));
                var_a2_3 += 2;
                (*(s8 *)((u8 *)(var_a1_4) + 1)) = temp_v0_3;
                var_a1_4 += 2;
            } while (var_a0_4 > 0);
            var_a2_4 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x1CE));
            var_a1_5 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x268));
            var_a0_5 = 0x4D;
            do {
                var_a0_5 -= 1;
                temp_v0_4 = (*(s8 *)((u8 *)(var_a2_4) + 1));
                (*(s8 *)((u8 *)(var_a1_5) + 0)) = (s8) (*(s8 *)((u8 *)(var_a2_4) + 0));
                var_a2_4 += 2;
                (*(s8 *)((u8 *)(var_a1_5) + 1)) = temp_v0_4;
                var_a1_5 += 2;
            } while (var_a0_5 > 0);
            var_a2_5 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x134));
            var_a1_6 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x1CE));
            var_a0_6 = 0x4D;
            do {
                var_a0_6 -= 1;
                temp_v0_5 = (*(s8 *)((u8 *)(var_a2_5) + 1));
                (*(s8 *)((u8 *)(var_a1_6) + 0)) = (s8) (*(s8 *)((u8 *)(var_a2_5) + 0));
                var_a2_5 += 2;
                (*(s8 *)((u8 *)(var_a1_6) + 1)) = temp_v0_5;
                var_a1_6 += 2;
            } while (var_a0_6 > 0);
            var_a2_6 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x9A));
            var_a1_7 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x134));
            var_a0_7 = 0x4D;
            do {
                var_a0_7 -= 1;
                temp_v0_6 = (*(s8 *)((u8 *)(var_a2_6) + 1));
                (*(s8 *)((u8 *)(var_a1_7) + 0)) = (s8) (*(s8 *)((u8 *)(var_a2_6) + 0));
                var_a2_6 += 2;
                (*(s8 *)((u8 *)(var_a1_7) + 1)) = temp_v0_6;
                var_a1_7 += 2;
            } while (var_a0_7 > 0);
            var_a2_7 = &RecvMailInfo;
            var_a1_8 = &(*(u8 *)((u8 *)&RecvMailInfo + 0x9A));
            var_a0_8 = 0x4D;
            do {
                var_a0_8 -= 1;
                temp_v0_7 = (*(s8 *)((u8 *)(var_a2_7) + 1));
                (*(s8 *)((u8 *)(var_a1_8) + 0)) = (s8) (*(u8 *)((u8 *)(var_a2_7) + 0));
                var_a2_7 += 2;
                (*(s8 *)((u8 *)(var_a1_8) + 1)) = temp_v0_7;
                var_a1_8 += 2;
            } while (var_a0_8 > 0);
            memset(&RecvMailInfo, 0, 0x9A);
            strncpy(&(*(u8 *)((u8 *)&RecvMailInfo + 0x1)), &sp40, 8);
            strncpy(&(*(u8 *)((u8 *)&RecvMailInfo + 0x9)), &sp48, 0x10);
            strncpy(&(*(u8 *)((u8 *)&RecvMailInfo + 0x1A)), &sp5C, 0x80);
            KinshiYogo_chk(&(*(u8 *)((u8 *)&RecvMailInfo + 0x1A)));
            temp_a0 = (*(void **)&g_main_2EFBAC);
            (*(s8 *)&RecvMailInfo) = 1;
            (*(u8 *)((u8 *)(temp_a0) + 0x234)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0x234)) + 1);
            temp_v1 = (*(void **)&g_main_2EFBAC);
            if ((s32) (*(u8 *)((u8 *)(temp_v1) + 0x234)) >= 9) {
                (*(u8 *)((u8 *)(temp_v1) + 0x234)) = 8U;
            }
        } else {
            goto loop_1;
        }
    }
}

