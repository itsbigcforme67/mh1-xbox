/* netfile2h - SLPM_654.95 0x00289AB0-0x0028A110 (SaveGameFileNet2): step machine (net_common_w.x7C) that saves the game
 * file to the memory card for the network menu. Returns 0 while running, 1 when done, -1 on failure. */
#include "netfile2.h"

int SaveGameFileNet2()
{
    int r;
    s16 t;

    r = 0;
    switch (net_common_w.x7C) {
    case 0:
        net_common_w.x7C++;
        net_common_w.x03 = 0;
        Net_McWorkInit(0);
        memset(data_load_ptr, 0, 0x16800);
        system_w.x3C = 1;
        break;
    case 1:
        Ncm_mssage_disp_req(8);
        if (Net_Icon_Data_Load(1) != 0) {
            net_common_w.x03 = 0;
            net_common_w.timer = 20;
            net_common_w.x7C++;
            net_common_w.x7A = McActAvailSet(data_load_ptr);
            McActSave0Set(net_common_w.x79, data_load_ptr, 1);
        }
        break;
    case 2:
        Ncm_mssage_disp_req(8);
        if (net_common_w.timer != 0) {
            net_common_w.timer--;
        } else {
            McActMain();
            net_common_w.x06 = McActResult();
            switch (net_common_w.x06) {
            case 0:
                if (McActNewChk(net_common_w.x79) != 0) {
                    net_common_w.x7C = 20;
                    net_common_w.timer = 20;
                } else if (save_data_load_game_for_net(0) != 0) {
                    net_common_w.x06 = -256;
                    goto fail;
                } else {
                    net_common_w.x7C = 3;
                    net_common_w.timer = 20;
                }
                break;
            case -1:
                break;
            case -256:
            case -251:
            default:
            fail:
                net_common_w.x7C = 60;
                net_common_w.timer = 20;
                break;
            case -255:
                net_common_w.timer = 20;
                net_common_w.x7C = 30;
                break;
            case -252:
            case -253:
            case -254:
                net_common_w.x7C = 10;
                net_common_w.timer = 20;
                break;
            }
        }
        break;
    case 3:
        net_common_w.x7C = 4;
        save_data_store_sys_foe_net();
        McActSaveSet(net_common_w.x79, data_load_ptr);
        break;
    case 4:
        Ncm_mssage_disp_req(0x48);
        McActMain();
        net_common_w.x06 = McActResult();
        switch (net_common_w.x06) {
        case 0:
            net_common_w.x7C = 5;
            decode_data_for_net(data_load_ptr);
            check_sum_set(card_w);
            break;
        case -1:
            break;
        default:
            net_common_w.x7D = 0;
            net_common_w.x7C = 60;
            break;
        }
        break;
    case 5:
        net_common_w.x7C++;
        net_common_w.timer = 300;
        system_w.x3C = 0;
    case 6:
        Ncm_mssage_disp_req(0x49);
        t = net_common_w.timer - 1;
        net_common_w.timer = t;
        if (t < 0xF1) {
            if (net_common_w.timer != 0) {
                if (net_shot_ok_ck(2) != 0) {
                    goto done5A;
                }
            } else {
            done5A:
                net_common_w.x7D = 0;
                net_common_w.x7C = 0x5A;
            }
        }
        break;
    case 10:
        net_common_w.x7C++;
        net_common_w.x06 = 0x744;
        system_w.x3C = 0;
        break;
    case 11:
        Ncm_mssage_disp_req(0x4A);
        dialog_limit_disp(0);
        if (net_shot_ok_ck(2) != 0) {
            net_common_w.x7C = 0x5B;
        } else if (net_common_w.x06 == 0) {
            net_common_w.x7C = 0x5C;
        }
        break;
    case 20:
        net_common_w.x7C++;
        net_common_w.x06 = 0x744;
        system_w.x3C = 0;
        break;
    case 21:
        Ncm_mssage_disp_req(0x69);
        dialog_limit_disp(0);
        if (net_shot_ok_ck(2) != 0) {
            net_common_w.x7C = 0x5B;
        } else if (net_common_w.x06 == 0) {
            net_common_w.x7C = 0x5C;
        }
        break;
    case 30:
        net_common_w.x7C++;
        net_common_w.x06 = 0x744;
        system_w.x3C = 0;
        break;
    case 31:
        Ncm_mssage_disp_req(0x4B);
        dialog_limit_disp(0);
        if (net_shot_ok_ck(2) != 0) {
            net_common_w.x7C = 0x5B;
        } else if (net_common_w.x06 == 0) {
            net_common_w.x7C = 0x5C;
        }
        break;
    case 60:
        net_common_w.x7C++;
        net_common_w.x06 = 0x744;
        system_w.x3C = 0;
        break;
    case 61:
        Ncm_mssage_disp_req(0x4C);
        dialog_limit_disp(0);
        if (net_shot_ok_ck(2) != 0) {
            net_common_w.x7C = 0x5B;
        } else if (net_common_w.x06 == 0) {
            net_common_w.x7C = 0x5C;
        }
        break;
    case 0x5A:
        Ncm_mssage_disp_req(0x49);
        r = 1;
    reset:
        system_w.x3C = 0;
        net_common_w.x7D = 0;
        net_common_w.x7C = 0;
        break;
    case 0x5B:
        r = -1;
        goto reset;
    case 0x5C:
        r = -1;
        goto reset;
    }
    return r;
}
