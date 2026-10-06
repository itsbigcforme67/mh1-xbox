/* NEAR-MATCH, not built: about 600 of 781 differ; original keeps the step in a2 (this build a1), which cascades. */
/* netfile2m - SLPM_654.95 0x002869A0-0x0028760C (NetFileLoad): step machine on net_common_w.sub that asks which card slot
 * to use, loads the net settings file from the card (McAct*), validates it (check_data_cn_file) and copies it to CNFile.
 * Returns 0 while running, 1 when finished, -1 when cancelled. */
#include "netfile2.h"

extern u8 CNFile[];

int NetFileLoad()
{
    int ret = 0;
    int sw;
    int r;

    switch (net_common_w.sub) {
    case 0:
        net_common_w.timer = 20;
        net_common_w.x03 = 0;
        net_common_w.sub++;
        net_common_w.x0A = net_common_w.x0D;
        Ncm_spr_D_MENU_set(11);
        Net_McWorkInit(2);
        system_w.x3C = 0;
        break;
    case 1:
        Ncm_mssage_disp_req(102);
        Ncm_menu_disp_req(11);
        if (net_common_w.timer != 0) {
            net_common_w.timer--;
            break;
        }
        sw = net_swdata();
        if (sw & 0x1000) {
            if (net_common_w.x0A != 1) {
                net_set_se_cur();
                net_common_w.x0A = 1;
            } else {
                net_set_se_cur();
                net_common_w.x0A = 0;
            }
        } else if (sw & 0x2000) {
            if (net_common_w.x0A != 0) {
                net_set_se_cur();
                net_common_w.x0A = 0;
            } else {
                net_set_se_cur();
                net_common_w.x0A = 1;
            }
        }
        if (net_shot_ok_ck(1) != 0) {
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill(0x100000);
            net_common_w.sub++;
            net_common_w.timer = 20;
            net_common_w.x0D = net_common_w.x0A;
            net_common_w.x79 = net_common_w.x0A;
            McActSave0Set(net_common_w.x0D, data_load_ptr, 0);
            system_w.x3C = 1;
        } else if (net_shot_ng_ck() != 0) {
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill2(0x100000);
            net_common_w.sub = 30;
            net_common_w.timer = 20;
        }
        break;
    case 2:
        Ncm_mssage_disp_req(102);
        if (net_common_w.timer != 0) {
            net_common_w.timer--;
            break;
        }
        McActMain();
        r = McActResult();
        switch (r) {
        case 0:
            net_common_w.sub++;
            net_common_w.timer = 20;
            McActCheckSet();
            net_common_w.x29 = 0;
            Ncm_spr_D_MENU_set(0, 2);
            break;
        case -255:
        case -254:
        case -253:
        case -252:
        case -256:
        case -251:
            net_common_w.x06 = r;
            system_w.x3C = 0;
            net_common_w.sub = 10;
            net_common_w.timer = 300;
            break;
        case -1:
            break;
        }
        break;
    case 3:
        Ncm_mssage_disp_req(115);
        Ncm_menu_disp_req(4);
        McActMain();
        if (McActConChk(net_common_w.x0D) == 0) {
            net_common_w.x29 = 0;
            net_common_w.sub = 40;
            net_common_w.timer = 300;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill2(0x100000);
            system_w.x3C = 0;
            break;
        }
        if (net_common_w.timer > 0) {
            net_common_w.timer--;
        }
        switch (net_yesno_operation_move()) {
        case 1:
            net_common_w.x03 = 0;
            net_common_w.sub++;
            net_common_w.timer = 20;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill(0x100000);
            break;
        case -1:
            net_common_w.sub = 0;
            net_common_w.timer = 0;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            if (net_shot_ok_ck(0) != 0) {
                Ncm_spr_kill(0x100000);
            } else {
                Ncm_spr_kill2(0x100000);
            }
            break;
        case 0:
            break;
        }
        break;
    case 4:
        Ncm_mssage_disp_req(77);
        McActMain();
        if (McActConChk(net_common_w.x0D) == 0) {
            net_common_w.x29 = 0;
            net_common_w.sub = 40;
            net_common_w.timer = 300;
            break;
        }
        net_common_w.timer--;
        if (net_common_w.timer > 0) {
            break;
        }
        net_common_w.sub++;
        net_common_w.timer = 20;
        net_common_w.x79 = net_common_w.x0D;
        McActLoadSet(net_common_w.x0D, data_load_ptr);
        break;
    case 5:
        Ncm_mssage_disp_req(77);
        if (net_common_w.timer != 0) {
            net_common_w.timer--;
            break;
        }
        McActMain();
        net_common_w.x06 = McActResult();
        switch (net_common_w.x06) {
        case -251:
        case -252:
        case -253:
        case -254:
        case -256:
        case -255:
            goto load_err;
        case 0:
            net_common_w.sub++;
            net_common_w.timer = 300;
            if (check_data_cn_file(data_load_ptr) != 0) {
                goto load_err;
            }
            memset(CNFile, 0, 7148);
            memcpy(CNFile, data_load_ptr, 7148);
            system_w.x3C = 0;
            break;
        }
        break;
    load_err:
        system_w.x3C = 0;
        net_common_w.sub = 20;
        net_common_w.timer = 300;
        break;
    case 6:
        Ncm_mssage_disp_req(78);
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.sub++;
            net_common_w.timer = 300;
            break;
        }
        if (net_common_w.timer < 271) {
            Ncm_mssage_disp_option_req(78);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.sub++;
                net_common_w.timer = 300;
            }
        }
        break;
    case 7:
        Ncm_mssage_disp_req(7);
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.sub++;
            break;
        }
        if (net_common_w.timer < 211) {
            Ncm_mssage_disp_option_req(7);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.sub++;
            }
        }
        break;
    case 8:
        ret = 1;
        break;
    case 10:
        switch (net_common_w.x06) {
        case -255:
            Ncm_mssage_disp_req(103);
            break;
        case -252:
        case -253:
        case -254:
            Ncm_mssage_disp_req(104);
            break;
        case -251:
            Ncm_mssage_disp_req(117);
            break;
        }
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.sub = 0;
            break;
        }
        if (net_common_w.timer < 271) {
            switch (net_common_w.x06) {
            case -255:
                Ncm_mssage_disp_option_req(103);
                break;
            case -252:
            case -253:
            case -254:
                Ncm_mssage_disp_option_req(104);
                break;
            case -251:
                Ncm_mssage_disp_option_req(117);
                break;
            }
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.sub = 0;
            }
        }
        break;
    case 20:
        Ncm_mssage_disp_req(79);
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.sub++;
            break;
        }
        if (net_common_w.timer < 271) {
            Ncm_mssage_disp_option_req(79);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.sub++;
                net_common_w.timer = 20;
            }
        }
        break;
    case 21:
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.sub = 0;
        }
        break;
    case 30:
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.sub++;
            net_common_w.x29 = 1;
            Ncm_spr_D_MENU_set(0, 2);
        }
        break;
    case 31:
        Ncm_mssage_disp_req(114);
        Ncm_menu_disp_req(4);
        switch (net_yesno_operation_move()) {
        case 1:
            net_common_w.x03 = 0;
            net_common_w.sub++;
            net_common_w.timer = 20;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill(0x100000);
            break;
        case -1:
            net_common_w.sub = 35;
            net_common_w.timer = 20;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            if (net_shot_ok_ck(0) != 0) {
                Ncm_spr_kill(0x100000);
            } else {
                Ncm_spr_kill2(0x100000);
            }
            break;
        case 0:
            break;
        }
        break;
    case 32:
        Ncm_mssage_disp_req(114);
        net_common_w.timer--;
        if (net_common_w.timer > 0) {
            break;
        }
        net_common_w.sub++;
        break;
    case 33:
        ret = -1;
        break;
    case 35:
        Ncm_mssage_disp_req(114);
        net_common_w.timer--;
        if (net_common_w.timer > 0) {
            break;
        }
        net_common_w.sub = 0;
        break;
    case 40:
        Ncm_mssage_disp_req(6);
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.sub = 0;
            net_common_w.timer = 0;
            break;
        }
        if (net_common_w.timer < 270) {
            Ncm_mssage_disp_option_req(6);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.sub = 0;
                net_common_w.timer = 0;
            }
        }
        break;
    }
    return ret;
}
