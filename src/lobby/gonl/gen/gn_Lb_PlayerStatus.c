/* G online (PC): Lb_PlayerStatus, written from Monster Hunter G's code at lobby 0x59DE40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 DispFrameList();
s32 DispFrameListOptionArrow();
s32 DispFrameMessage();
s32 PlayerEquipmentWindow();
s32 PrintPlayerJob();
s32 flfntLocate();
s32 flfntSetSize();
s32 font_print_uf();
s32 font_set_palette();
s32 func_0012DE30();
s32 lobby_func_0059DA90();
extern u8 g_main_2EF074;
extern u8 g_sub_527D34;
extern u8 cw;
extern u8 g_main_2EF074;
extern u8 g_sub_527D34;
extern u8 menu_stat_job_str;
extern u8 net_etc_mes_tbl;
void *GetAdrsMiniData();                          /* extern */
s32 font_print();                        /* extern */
s32 func_00223610();    /* extern */
s32 sprintf();               /* extern */
extern s32 frame_status_main_0064DDC0;
extern s32 frame_status_sub_0064DDF0;
extern s32 hunter_appellation;

s32 Lb_PlayerStatus(void **arg0, s32 arg1) {
    u8 g_frame[0x90] __attribute__((aligned(16)));   /* G's stack frame */
#define sp50 (*(s32 *)(g_frame + 0x50))
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_s2;
    s32 var_s0;
    u8 temp_a1;
    u8 *temp_s0_2;
    u8 *temp_s3;

    temp_s2 = arg1 & 0xFF & 0x80;
    temp_s3 = *arg0;
    var_s0 = arg1;
    if (temp_s2 != 0) {
        var_s0 &= 0xF;
    }
    if ((*(u8 *)((u8 *)(temp_s3) + 0)) == 0) {
        DispFrameList((u8 *)&frame_status_main_0064DDC0 + ((var_s0 & 0xFF) * 0x18), *(void **)0x2EF070, -1);
        return -1;
    }
    temp_s1 = var_s0 & 0xFF;
    sprintf(&sp50, (*(s32 *)&g_main_2EF074), temp_s1 + 1);
    temp_s0 = var_s0 & 0xFF;
    DispFrameList((u8 *)&frame_status_main_0064DDC0 + (temp_s0 * 0x18), &sp50, -1);
    if (temp_s2 == 0) {
        DispFrameListOptionArrow(&frame_status_main_0064DDC0);
    }
    DispFrameMessage((u8 *)&frame_status_sub_0064DDF0 + (temp_s0 * 0x10), *(&(*(u8 *)((u8 *)&menu_stat_job_str + 0x20)) + (temp_s0 * 4)));
    temp_s0_2 = GetAdrsMiniData((*(u8 *)((u8 *)(temp_s3) + 0xC)));
    switch (temp_s1) {                              /* irregular */
    case 0:
        font_set_palette(0);
        flfntLocate(0x17A, 0x52);
        font_print_uf((*(s32 *)&cw) + ((u16) (*(u8 *)((u8 *)(temp_s3) + 0xC)) * 0x5C) + 0x394);
        flfntLocate(0x17A, 0x66);
        font_print_uf((*(s32 *)&cw) + ((u16) (*(u8 *)((u8 *)(temp_s3) + 0xC)) * 0x5C) + 0x38C);
        flfntLocate(0x17A, 0x7A);
        PrintPlayerJob(temp_s3);
        flfntLocate(0x17A, 0x8E);
        temp_a1 = (*(u8 *)((u8 *)(temp_s0_2) + 1));
        if (temp_a1 != 0) {
            font_print((*(s32 *)((u8 *)&menu_stat_job_str + 0x44)), temp_a1);
            flfntLocate(0x18C, 0x8E);
            if (lobby_func_0059DA90((*(s32 *)((u8 *)&hunter_appellation + ((*(u8 *)((u8 *)(temp_s0_2) + 1)) * 4))), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) >= 0x13) {
                flfntSetSize(0x11, 0x12);
            }
            font_print((*(s32 *)&g_sub_527D34), (u8) (*(s32 *)((u8 *)&hunter_appellation + ((*(u8 *)((u8 *)(temp_s0_2) + 1)) * 4))));
            flfntSetSize(0x12, 0x12);
        } else {
            font_print_uf((*(void * *)((u8 *)&net_etc_mes_tbl + 0x20)));
        }
        func_0012DE30(0x132, 0xAE, 0x14, (*(s32 *)&cw) + ((u16) (*(u8 *)((u8 *)(temp_s3) + 0xC)) * 0x62) + 0x724);
        break;
    case 1:
        PlayerEquipmentWindow(temp_s3);
        func_00223610(temp_s3, 0x132, 0x126);
        break;
    }
    return 0;
}

