/* netfile2k - SLPM_654.95 0x0028AB30-0x0028B204 (SaveNetFileBr): the same net settings file save as SaveNetFile_ForLobby
 * but for the bar/menu variant (dialogs through net_shot_ok_ck, remaining-time digits copied to net_common_w.x8D by
 * dialog_limit_disp mode 2). Step machine on net_common_w.x7C; returns 0 while running, 1 when saved, -1 when given up. */
#include "netfile2.h"

extern u8 CNFile[];

int SaveNetFileBr()
{
    int r;
    s16 t;
    s8 s;

    s = net_common_w.x7C;
    r = 0;
    switch (s) {
    case 0:
        net_common_w.x03 = 0;
        Net_McWorkInit(2);
        net_common_w.x8C = 0;
        net_common_w.x0D = net_sel_drive;
        net_common_w.x79 = net_common_w.x0D;
        dialog_open_set(1, 0x1D);
        break;
    case 1:
        if (Net_Icon_Data_Load(0) != 0) {
            net_common_w.x7D = 0;
            net_common_w.x7C = net_common_w.x7C + 1;
            net_common_w.x7A = McActAvailSet((int)data_load_ptr + 0x12000);
            McActSave0Set(net_common_w.x0D, data_load_ptr, 0);
        }
        break;
    case 2:
        McActMain();
        net_common_w.x0A = McActResult();
        switch (net_common_w.x0A) {
        case 0:
            net_common_w.x7C++;
            break;
        case -256:
        case -251:
            net_common_w.x7C = 0x3C;
            break;
        case -255:
            net_common_w.x7C = 0x1E;
            break;
        case -252:
        case -253:
        case -254:
            net_common_w.x7C = 0x46;
            break;
        }
        net_common_w.x7D = 0;
        break;
    case 3:
        dialog_open_set(4, 0x31);
        memset(data_load_ptr, 0, 0x1BEC);
        memcpy(data_load_ptr, CNFile, 0x1BEC);
        check_sum_set_cn_file(data_load_ptr);
        McActSaveSet(net_common_w.x0D, data_load_ptr);
        break;
    case 4:
        McActMain();
        net_common_w.x0A = McActResult();
        if (net_common_w.x0A != -1) {
            dialog_close_set(5);
        }
        break;
    case 5:
        switch (net_common_w.x0A) {
        case 0:
            dialog_open_set(6, 0x32);
            break;
        default:
            net_common_w.x7C = 0x3C;
            net_common_w.x7D = 0;
            break;
        }
        break;
    case 6:
        net_common_w.x7C = s + 1;
        net_common_w.timer = 300;
    case 7:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t < 0xF1) {
            if (net_common_w.timer != 0) {
                if (net_shot_ok_ck(2) != 0) {
                    goto close5A;
                }
            } else {
            close5A:
                dialog_close_set(0x5A);
            }
        }
        break;
    case 0x1E:
        dialog_open_set(0x1F, 0x2C);
        McActCheckSet();
        break;
    case 0x1F:
        dialog_limit_disp(2);
        McActMain();
        if (McActConChk(net_common_w.x79) != 0) {
            dialog_close_set(0);
        } else if (net_shot_ok_ck(2) == 0) {
            if (net_common_w.x06 == 0) {
                goto close5C_a;
            }
        } else {
        close5C_a:
            dialog_close_set(0x5C);
        }
        break;
    case 0x3C:
        dialog_open_set(0x3D, 0x23);
        break;
    case 0x3D:
        dialog_limit_disp(2);
        if (net_shot_ok_ck(2) != 0) {
            dialog_close_set(0x5B);
        } else if (net_common_w.x06 == 0) {
            dialog_close_set(0x5C);
        }
        break;
    case 0x46:
        dialog_open_set(0x47, 0x72);
        McActCheckSet();
        break;
    case 0x47:
        dialog_limit_disp(2);
        McActMain();
        if (McActConChk(net_common_w.x79) == 0) {
            dialog_close_set(0x1F);
        } else if (net_shot_ok_ck(2) == 0) {
            if (net_common_w.x06 == 0) {
                goto close5C_b;
            }
        } else {
        close5C_b:
            dialog_close_set(0x5C);
        }
        break;
    case 0x5A:
        r = 1;
    done:
        net_common_w.x7D = 0;
        net_common_w.x7C = 0;
        net_common_w.timer = 0;
        net_common_w.x06 = 0;
        break;
    case 0x5B:
        r = -1;
        goto done;
    case 0x5C:
        r = -1;
        goto done;
    case 0x64:
        net_common_w.x7C = s + 1;
        func_5E5D20(mc_bs_chg(net_common_w.x7E));
        break;
    case 0x65:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t == 0) {
            net_common_w.x7C = net_common_w.x7D;
            net_common_w.x7D = 0;
        }
        break;
    case 0x6E:
        net_common_w.x7C = s + 1;
        net_common_w.timer = 8;
        func_5E5D30();
        net_common_w.x7E = 0;
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
        net_common_w.x7C = s + 1;
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
        net_common_w.x7C = s + 1;
        net_common_w.x7E = 0;
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
