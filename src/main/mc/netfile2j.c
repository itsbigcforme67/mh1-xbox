/* netfile2j - SLPM_654.95 0x0028A3D0-0x0028AB2C (SaveNetFile_ForLobby): step machine (net_common_w.x7C) that creates the
 * net settings file on the memory card for the lobby, driven through dialog_open_set/dialog_close_set (x7D = step to
 * return to after the dialog). Returns 0 while running, 1 when saved, -1 when given up. */
#include "netfile2.h"

extern u8 CNFile[];

/* card_w + 0x18: the selected drive */
#define CARD_DRIVE (*(s32 *)(card_w + 0x18))

int SaveNetFile_ForLobby()
{
    int r;
    s16 t;

    r = 0;
    switch (net_common_w.x7C) {
    case 0:
        net_common_w.x7C++;
        net_common_w.x03 = 0;
        Net_McWorkInit(2);
        net_common_w.x0D = net_sel_drive;
        Last_sel_drive = CARD_DRIVE;
        net_common_w.x79 = net_common_w.x0D;
        CARD_DRIVE = (s8)net_common_w.x79;
        memset(data_load_ptr, 0, 0x1BEC);
        memcpy(data_load_ptr, CNFile, 0x1BEC);
        dialog_open_set(1, 0x3A);
        break;
    case 1:
        *(s8 *)0x6B2A2C = 1;
        if (Net_Icon_Data_Load(0) != 0) {
            net_common_w.x03 = 0;
            net_common_w.timer = 20;
            net_common_w.x7C++;
            net_common_w.x7A = McActAvailSet((int)data_load_ptr + 0x12000);
            McActSave0Set(net_common_w.x0D, data_load_ptr, 0);
        }
        break;
    case 2:
        *(s8 *)0x6B2A2C = 1;
        McActMain();
        net_common_w.x0A = McActResult();
        switch (net_common_w.x0A) {
        case 0:
            dialog_close_set(3);
            break;
        case -1:
            break;
        case -256:
        case -251:
            dialog_close_set(0x3C);
            break;
        case -255:
            dialog_close_set(0x1E);
            break;
        case -252:
        case -253:
        case -254:
            dialog_close_set(0x46);
            break;
        }
        break;
    case 3:
        dialog_open_set(4, 0x3B);
        memset(data_load_ptr, 0, 0x1BEC);
        memcpy(data_load_ptr, CNFile, 0x1BEC);
        check_sum_set_cn_file(data_load_ptr);
        McActSaveSet(net_common_w.x0D, data_load_ptr);
        break;
    case 4:
        *(s8 *)0x6B2A2C = 1;
        McActMain();
        net_common_w.x0A = McActResult();
        if (net_common_w.x0A != -1) {
            dialog_close_set(5);
        }
        break;
    case 5:
        switch (net_common_w.x0A) {
        case 0:
            dialog_open_set(6, 0x3C);
            break;
        default:
            net_common_w.x7C = 0x3C;
            net_common_w.x7D = 0;
            break;
        }
        break;
    case 6:
        net_common_w.x7C++;
        net_common_w.timer = 300;
    case 7:
        *(s8 *)0x6B2A2C = 1;
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t < 0xF1 && (net_common_w.timer == 0 || net_joy_ok_ck_each(0) != 0)) {
            dialog_close_set(0x5A);
        }
        break;
    case 0x1E:
        dialog_open_set(0x1F, 0x3D);
        McActCheckSet();
        break;
    case 0x1F:
        *(s8 *)0x6B2A2C = 1;
        dialog_limit_disp(1);
        McActMain();
        if (McActConChk(net_common_w.x0D) != 0) {
            dialog_close_set(0);
        } else if (net_joy_ok_ck_each(0) == 0) {
            if (net_common_w.x06 == 0) {
                goto close5C_a;
            }
        } else {
        close5C_a:
            dialog_close_set(0x5C);
        }
        break;
    case 0x29:
        *(s8 *)0x6B2A2C = 1;
        dialog_limit_disp(1);
        McActMain();
        if (McActConChk(net_common_w.x0D) == 0) {
            net_common_w.x7C = 0x1F;
        } else if (net_joy_ok_ck_each(0) == 0) {
            if (net_common_w.x06 == 0) {
                goto close5C_b;
            }
        } else {
        close5C_b:
            dialog_close_set(0x5C);
        }
        break;
    case 0x33:
        *(s8 *)0x6B2A2C = 1;
        dialog_limit_disp(1);
        *(s8 *)0x6B2A2C = 1;
        if (net_joy_ok_ck_each(0) != 0) {
            dialog_close_set(0x5B);
        } else if (net_common_w.x06 == 0) {
            dialog_close_set(0x5C);
        }
        break;
    case 0x3C:
        dialog_open_set(0x33, 0x3E);
        break;
    case 0x46:
        dialog_open_set(0x29, 0x3F);
        McActCheckSet();
        break;
    case 0x5A:
        r = 1;
    done:
        net_common_w.x7D = 0;
        net_common_w.x7C = 0;
        CARD_DRIVE = Last_sel_drive;
        break;
    case 0x5B:
        r = -1;
        goto done;
    case 0x5C:
        r = -1;
        goto done;
    case 0x64:
        net_common_w.x7C++;
        func_591BE0(net_common_w.x7E, 0);
        break;
    case 0x65:
        *(s8 *)0x6B2A2C = 1;
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t == 0) {
            net_common_w.x7C = net_common_w.x7D;
            net_common_w.x7D = 0;
        }
        break;
    case 0x6E:
        net_common_w.x7C++;
        net_common_w.timer = 8;
        break;
    case 0x6F:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t == 0) {
            net_common_w.x7C = net_common_w.x7D;
            net_common_w.x7D = 0;
        }
        break;
    case 0x78:
        net_common_w.x7C++;
        break;
    case 0x79:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t == 0) {
            net_common_w.x7C = net_common_w.x7D;
            net_common_w.x7D = 0;
        }
        break;
    case 0x82:
        net_common_w.x7C++;
        break;
    case 0x83:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t == 0) {
            net_common_w.x7C = 0x6E;
        }
        break;
    }
    return r;
}
