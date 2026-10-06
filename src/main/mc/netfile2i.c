/* netfile2i - SLPM_654.95 0x00289120-0x00289AAC (SaveNetFile): step machine (net_common_w.sub) that creates/saves the
 * net settings file (CNFile, 0x1BEC bytes) on the memory card, with the card-removed and error dialogs.
 * Returns 0 while running, 1 when saved, -1 when given up. */
#include "netfile2.h"

extern s32 net_sel_drive;
extern u8 CNFile[];
void check_sum_set_cn_file();

int SaveNetFile()
{
    int r;
    int res;
    s16 t;
    u8 u;

    r = 0;
    switch (net_common_w.sub) {
    case 0:
        net_common_w.sub++;
        net_common_w.x03 = 0;
        Net_McWorkInit(2);
        net_common_w.x0D = net_sel_drive;
        net_common_w.x79 = net_common_w.x0D;
        McActCheckSet();
        McActMain();
        system_w.x3C = 1;
        break;
    case 1:
        if (Net_Icon_Data_Load(0) != 0) {
            net_common_w.x03 = 0;
            net_common_w.timer = 20;
            net_common_w.sub++;
            net_common_w.x7A = McActAvailSet((int)data_load_ptr + 0x12000);
            McActSave0Set(net_common_w.x0D, data_load_ptr, 0);
        }
        break;
    case 2:
        Ncm_mssage_disp_req(8);
        if (net_common_w.timer != 0) {
            net_common_w.timer--;
        } else {
            McActMain();
            res = McActResult();
            switch (res) {
            case 0:
                u = net_common_w.sub;
                net_common_w.timer = 20;
                net_common_w.sub = u + 1;
                net_common_w.x79 = net_common_w.x0D;
                memset(data_load_ptr, 0, 0x1BEC);
                memcpy(data_load_ptr, CNFile, 0x1BEC);
                check_sum_set_cn_file(data_load_ptr);
                McActSaveSet(net_common_w.x0D, data_load_ptr);
                break;
            case -1:
                break;
            case -255:
                net_common_w.sub = 20;
                net_common_w.x06 = 0x744;
                net_common_w.timer = 30;
                McActCheckSet();
                break;
            case -256:
                system_w.x3C = 0;
                net_common_w.sub = 40;
                net_common_w.timer = 300;
                break;
            case -251:
                system_w.x3C = 0;
                net_common_w.sub = 50;
                net_common_w.timer = 300;
                break;
            default:
                net_common_w.sub = 30;
                net_common_w.x06 = 0x744;
                net_common_w.timer = 30;
                McActCheckSet();
                break;
            }
        }
        break;
    case 3:
        Ncm_mssage_disp_req(9);
        if (net_common_w.timer != 0) {
            net_common_w.timer--;
        } else {
            McActMain();
            res = McActResult();
            switch (res) {
            case 0:
                system_w.x3C = 0;
                u = net_common_w.sub;
                net_common_w.timer = 300;
                net_common_w.sub = u + 1;
                break;
            case -1:
                break;
            case -255:
                net_common_w.sub = 20;
                net_common_w.x06 = 0x744;
                net_common_w.timer = 30;
                McActCheckSet();
                break;
            case -256:
            case -251:
                system_w.x3C = 0;
                net_common_w.sub = 40;
                net_common_w.timer = 300;
                break;
            }
        }
        break;
    case 4:
        Ncm_mssage_disp_req(10);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t > 0) {
            if (net_common_w.timer < 0x10F) {
                Ncm_mssage_disp_option_req(10);
                if (net_shot_ok_ck(2) != 0) {
                    net_common_w.sub++;
                }
            }
        } else {
            net_common_w.sub++;
        }
        break;
    case 5:
        r = 1;
        break;
    case 20:
        Ncm_mssage_disp_req(13);
        Ncm_mssage_disp_req(14);
        dialog_limit_disp(0);
        if (net_common_w.x06 > 0) {
            if (net_common_w.timer > 0) {
                if (net_shot_ok_ck(2) != 0) {
                    net_common_w.sub++;
                    McActStopSet();
                    Ncm_spr_kill(0x200);
                    system_w.x3C = 0;
                } else {
                    goto l20;
                }
            } else {
                net_common_w.timer--;
            l20:
                McActMain();
                res = McActConChk(net_common_w.x0D);
                switch (res) {
                case 0:
                    break;
                case 1:
                case 2:
                case 3:
                    McActStopSet();
                    net_common_w.sub = 25;
                    net_common_w.timer = 10;
                    break;
                }
            }
        } else {
            net_common_w.sub++;
            McActStopSet();
            Ncm_spr_kill(0x200);
            system_w.x3C = 0;
        }
        break;
    case 21:
        r = -1;
        break;
    case 25:
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t > 0) {
            McActMain();
        } else {
            net_common_w.x03 = 0;
            net_common_w.sub = 2;
            net_common_w.timer = 20;
            McActSave0Set(net_common_w.x0D, data_load_ptr, 0);
        }
        break;
    case 30:
        Ncm_mssage_disp_req(15);
        Ncm_mssage_disp_req(16);
        dialog_limit_disp(0);
        if (net_common_w.x06 > 0) {
            if (net_common_w.timer > 0) {
                if (net_shot_ok_ck(2) != 0) {
                    net_common_w.sub = 21;
                    McActStopSet();
                    Ncm_spr_kill(0x200);
                    system_w.x3C = 0;
                } else {
                    goto l30;
                }
            } else {
                net_common_w.timer--;
            l30:
                McActMain();
                res = McActConChk(net_common_w.x0D);
                switch (res) {
                case 0:
                    net_common_w.sub = 35;
                    McActCheckSet();
                    break;
                case 2:
                case 1:
                case 3:
                    break;
                }
            }
        } else {
            net_common_w.sub = 21;
            McActStopSet();
            Ncm_spr_kill(0x200);
            system_w.x3C = 0;
        }
        break;
    case 35:
        Ncm_mssage_disp_req(15);
        Ncm_mssage_disp_req(16);
        dialog_limit_disp(0);
        if (net_common_w.x06 > 0) {
            if (net_common_w.timer > 0) {
                if (net_shot_ok_ck(2) != 0) {
                    net_common_w.sub = 21;
                    McActStopSet();
                    Ncm_spr_kill(0x200);
                    system_w.x3C = 0;
                } else {
                    goto l35;
                }
            } else {
                net_common_w.timer--;
            l35:
                McActMain();
                res = McActConChk(net_common_w.x0D);
                switch (res) {
                case 1:
                case 2:
                case 3:
                    McActStopSet();
                    net_common_w.sub = 25;
                    net_common_w.timer = 10;
                    break;
                case 0:
                    break;
                }
            }
        } else {
            net_common_w.sub = 21;
            McActStopSet();
            Ncm_spr_kill(0x200);
            system_w.x3C = 0;
        }
        break;
    case 40:
        Ncm_mssage_disp_req(11);
        Ncm_mssage_disp_req(12);
        dialog_limit_disp(0);
        if (net_common_w.x06 > 0) {
            if (net_common_w.timer > 0) {
                if (net_shot_ok_ck(2) != 0) {
                    net_common_w.sub = 41;
                    Ncm_spr_kill(0x200);
                    system_w.x3C = 0;
                }
            } else {
                net_common_w.timer--;
            }
        } else {
            net_common_w.sub = 41;
            Ncm_spr_kill(0x200);
            system_w.x3C = 0;
        }
        break;
    case 41:
        r = -1;
        break;
    case 50:
        Ncm_mssage_disp_req(0x75);
        Ncm_mssage_disp_req(12);
        dialog_limit_disp(0);
        if (net_common_w.x06 > 0) {
            if (net_common_w.timer > 0) {
                if (net_shot_ok_ck(2) != 0) {
                    net_common_w.sub = 51;
                    Ncm_spr_kill(0x200);
                    system_w.x3C = 0;
                }
            } else {
                net_common_w.timer--;
            }
        } else {
            net_common_w.sub = 51;
            Ncm_spr_kill(0x200);
            system_w.x3C = 0;
        }
        break;
    case 51:
        r = -1;
        break;
    }
    return r;
}
