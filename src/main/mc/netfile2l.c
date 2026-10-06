/* netfile2l - SLPM_654.95 0x0028B580-0x0028BEC0 (NetAutoLoad): step machine (net_common_w.sub) that loads the game file
 * from memory card slot 0, then slot 1, remembering per slot what failed (err_status_no_card / no_file / ng_err bit per
 * slot) and showing the matching message. Returns 0 while running, 1 when loaded, -1 when nothing could be loaded. */
#include "netfile2.h"

extern s8 err_status_ng_err;
extern s8 err_status_no_card;
extern s8 err_status_no_file;
int McActInit();

int NetAutoLoad()
{
    int r;
    s16 t;
    int res;
    u8 u;

    r = 0;
    switch (net_common_w.sub) {
    case 0:
        net_common_w.timer = 20;
        net_common_w.x03 = 0;
        err_status_no_card = 0;
        net_common_w.sub++;
        err_status_no_file = 0;
        net_common_w.x79 = 0;
        err_status_ng_err = 0;
        McActInit(0);
        system_w.x3C = 1;
        break;
    case 1:
        Ncm_mssage_disp_req(0x5F);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t > 0) {
            net_common_w.sub++;
            McActSave0Set(net_common_w.x79, data_load_ptr, 1);
        }
        break;
    case 2:
        Ncm_mssage_disp_req(0x5F);
        McActMain();
        net_common_w.x06 = McActResult();
        switch (net_common_w.x06) {
        case -251:
        case 0:
            net_common_w.sub++;
            net_common_w.timer = 20;
        case -1:
            return 0;
        case -255:
            err_status_no_card |= 1 << net_common_w.x79;
            break;
        case -254:
        case -253:
        case -252:
            err_status_no_file |= 1 << net_common_w.x79;
            break;
        case -256:
            err_status_ng_err |= 1 << net_common_w.x79;
            break;
        }
        if (net_common_w.x79 == 0) {
            net_common_w.x79++;
            net_common_w.sub = 1;
            net_common_w.timer = 10;
        } else if (err_status_no_card == 3) {
            net_common_w.sub = 10;
            net_common_w.timer = 20;
        } else if ((err_status_no_card | err_status_no_file) == 3) {
            net_common_w.sub = 20;
            net_common_w.timer = 20;
        } else {
            net_common_w.x79 = ((err_status_ng_err & 1) != 0) ^ 1;
            net_common_w.sub = 40;
            net_common_w.timer = 20;
        }
        break;
    case 3:
        Ncm_mssage_disp_req(0x60);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t > 0) {
            net_common_w.sub++;
            McActLoadSet(net_common_w.x79, data_load_ptr);
        }
        break;
    case 4:
        McActMain();
        net_common_w.x06 = McActResult();
        switch (net_common_w.x06) {
        case 0:
            if (save_data_load_game_for_net(1) == 0) {
                system_w.x3C = 0;
                net_common_w.timer = 300;
                net_common_w.sub++;
                Last_sel_drive = net_common_w.x79;
            case -1:
                return 0;
            }
            goto e256;
        case -255:
            net_common_w.sub = 10;
            net_common_w.timer = 20;
            break;
        case -254:
        case -253:
        case -252:
            net_common_w.sub = 20;
            net_common_w.timer = 20;
            break;
        case -256:
        e256:
            net_common_w.sub = 30;
            net_common_w.timer = 20;
            break;
        }
        break;
    case 5:
        Ncm_mssage_disp_req(0x61);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            system_w.x3C = 0;
            net_common_w.sub++;
        } else if (net_common_w.timer < 0xF0) {
            Ncm_mssage_disp_option_req(0x61);
            if (net_shot_ok_ck(2) != 0) {
                system_w.x3C = 0;
                net_common_w.sub++;
            }
        }
        break;
    case 6:
        r = 1;
        break;
    case 10:
        Ncm_mssage_disp_req(0x64);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            u = net_common_w.sub;
            net_common_w.timer = 300;
            net_common_w.sub = u + 1;
        }
        break;
    case 11:
        Ncm_mssage_disp_req(0x64);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.sub++;
        } else if (net_common_w.timer < 0xF0) {
            Ncm_mssage_disp_option_req(0x64);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.sub++;
            }
        }
        break;
    case 12:
        system_w.x3C = 0;
        r = -1;
        break;
    case 20:
        Ncm_mssage_disp_req(0x63);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            u = net_common_w.sub;
            net_common_w.timer = 300;
            net_common_w.sub = u + 1;
        }
        break;
    case 21:
        Ncm_mssage_disp_req(0x63);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.sub++;
        } else if (net_common_w.timer < 0xF0) {
            Ncm_mssage_disp_option_req(0x63);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.sub++;
            }
        }
        break;
    case 22:
        r = -1;
        break;
    case 30:
        if (net_common_w.x06 == 0) {
            Ncm_mssage_disp_req(0x78);
        } else {
            Ncm_mssage_disp_req(0x62);
        }
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            u = net_common_w.sub;
            net_common_w.timer = 300;
            net_common_w.sub = u + 1;
        }
        break;
    case 31:
        if (net_common_w.x06 == 0) {
            Ncm_mssage_disp_req(0x78);
        } else {
            Ncm_mssage_disp_req(0x62);
        }
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.sub++;
        } else if (net_common_w.timer < 0xF0) {
            if (net_common_w.x06 == 0) {
                Ncm_mssage_disp_option_req(0x78);
            } else {
                Ncm_mssage_disp_option_req(0x62);
            }
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.sub++;
            }
        }
        break;
    case 32:
        r = -1;
        break;
    case 40:
        Ncm_mssage_disp_req(0x6A);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            u = net_common_w.sub;
            net_common_w.timer = 300;
            net_common_w.sub = u + 1;
        }
        break;
    case 41:
        Ncm_mssage_disp_req(0x6A);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t <= 0) {
            net_common_w.sub++;
        } else if (net_common_w.timer < 0xF0) {
            Ncm_mssage_disp_option_req(0x6A);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.sub++;
            }
        }
        break;
    case 42:
        r = -1;
        break;
    }
    return r;
}
