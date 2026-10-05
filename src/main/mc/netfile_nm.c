/* Memory card "net file" (online game save/CN file) screens, SLPM_654.95 main 0x2869A0-0x28BEC0.
 * NetFileLoad: state machine on net_common_w.step (u8 at +2) that asks the player which card slot to use, loads the
 * net file from the card (McAct*), validates it (check_data_cn_file) and copies it to CNFile. The online lobby code
 * is not part of the port, so these are kept as near-match C (field names are guesses). Result: 0 running,
 * 1 finished, -1 cancelled. */
#include "types.h"

typedef struct NCW {
    u8 _pad00[2];
    u8 step;        /* 0x02 */
    u8 x03;         /* 0x03 */
    s16 timer;      /* 0x04 */
    s16 err;        /* 0x06 McActResult code of the failure */
    u8 _pad08[2];
    s16 cursor;     /* 0x0A selected slot / yes-no */
    u8 _pad0C;
    u8 port;        /* 0x0D memory card port */
    u8 _pad0E[0x29 - 0x0E];
    u8 x29;         /* 0x29 */
    u8 _pad2A[0x79 - 0x2A];
    u8 x79;         /* 0x79 */
} NCW;

extern NCW net_common_w;
extern u8 system_w[];
extern u8 *data_load_ptr;
extern u8 CNFile[];

void Ncm_spr_D_MENU_set();
void Net_McWorkInit();
void Ncm_mssage_disp_req();
void Ncm_mssage_disp_option_req();
void Ncm_menu_disp_req();
int net_swdata();
void net_set_se_cur();
int net_shot_ok_ck();
int net_shot_ng_ck();
void Ncm_spr_kill();
void Ncm_spr_kill2();
int net_yesno_operation_move();
void McActMain();
int McActResult();
int McActConChk();
void McActCheckSet();
void McActLoadSet();
void McActSave0Set();
int check_data_cn_file();
void *memset();
void *memcpy();

int NetFileLoad(void)
{
    s16 ret = 0;
    int r;

    switch (net_common_w.step) {
    case 0:
        net_common_w.timer = 20;
        net_common_w.x03 = 0;
        net_common_w.step++;
        net_common_w.cursor = net_common_w.port;
        Ncm_spr_D_MENU_set(11);
        Net_McWorkInit(2);
        system_w[0x3C] = 0;
        break;
    case 1:
        Ncm_mssage_disp_req(102);
        Ncm_menu_disp_req(11);
        if (net_common_w.timer != 0) {
            net_common_w.timer--;
            break;
        }
        if (net_swdata() & 0x1000) {
            if (net_common_w.cursor == 1) {
                net_set_se_cur();
                net_common_w.cursor = 0;
            } else {
                net_set_se_cur();
                net_common_w.cursor = 1;
            }
        } else if (net_swdata() & 0x2000) {
            if (net_common_w.cursor == 0) {
                net_set_se_cur();
                net_common_w.cursor = 1;
            } else {
                net_set_se_cur();
                net_common_w.cursor = 0;
            }
        }
        if (net_shot_ok_ck(1) != 0) {
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill(0x100000);
            net_common_w.timer = 20;
            net_common_w.step++;
            net_common_w.port = net_common_w.cursor;
            net_common_w.x79 = net_common_w.cursor;
            McActSave0Set(net_common_w.port, data_load_ptr, 0);
            system_w[0x3C] = 1;
        } else if (net_shot_ng_ck() != 0) {
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill2(0x100000);
            net_common_w.step = 30;
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
        case -1:
            break;
        case -251:
        case -256:
        case -252:
        case -253:
        case -254:
        case -255:
            net_common_w.err = r;
            system_w[0x3C] = 0;
            net_common_w.step = 10;
            net_common_w.timer = 300;
            break;
        case 0:
            net_common_w.timer = 20;
            net_common_w.step++;
            McActCheckSet();
            net_common_w.x29 = 0;
            Ncm_spr_D_MENU_set(0, 2);
            break;
        }
        break;
    case 3:
        Ncm_mssage_disp_req(115);
        Ncm_menu_disp_req(4);
        McActMain();
        if (McActConChk(net_common_w.port) == 0) {
            net_common_w.x29 = 0;
            net_common_w.step = 40;
            net_common_w.timer = 300;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill2(0x100000);
            system_w[0x3C] = 0;
            break;
        }
        if (net_common_w.timer > 0) {
            net_common_w.timer--;
        }
        switch (net_yesno_operation_move()) {
        case 1:
            net_common_w.x03 = 0;
            net_common_w.timer = 20;
            net_common_w.step++;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill(0x100000);
            break;
        case -1:
            net_common_w.step = 0;
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
        if (McActConChk(net_common_w.port) == 0) {
            net_common_w.x29 = 0;
            net_common_w.step = 40;
            net_common_w.timer = 300;
            break;
        }
        net_common_w.timer--;
        if (net_common_w.timer > 0) {
            break;
        }
        net_common_w.timer = 20;
        net_common_w.step++;
        net_common_w.x79 = net_common_w.port;
        McActLoadSet(net_common_w.port, data_load_ptr);
        break;
    case 5:
        Ncm_mssage_disp_req(77);
        if (net_common_w.timer != 0) {
            net_common_w.timer--;
            break;
        }
        McActMain();
        r = McActResult();
        switch (r) {
        case -251:
        case -252:
        case -253:
        case -254:
        case -256:
        case -255:
            goto load_err;
        case 0:
            net_common_w.timer = 300;
            net_common_w.step++;
            if (check_data_cn_file(data_load_ptr) != 0) {
                goto load_err;
            }
            memset(CNFile, 0, 7148);
            memcpy(CNFile, data_load_ptr, 7148);
            system_w[0x3C] = 0;
            break;
        }
        break;
    load_err:
        net_common_w.err = r;
        system_w[0x3C] = 0;
        net_common_w.step = 20;
        net_common_w.timer = 300;
        break;
    case 6:
        Ncm_mssage_disp_req(78);
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.timer = 300;
            net_common_w.step++;
            break;
        }
        if (net_common_w.timer < 271) {
            Ncm_mssage_disp_option_req(78);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.timer = 300;
                net_common_w.step++;
            }
        }
        break;
    case 7:
        Ncm_mssage_disp_req(7);
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.step++;
            break;
        }
        if (net_common_w.timer < 211) {
            Ncm_mssage_disp_option_req(7);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.step++;
            }
        }
        break;
    case 8:
        ret = 1;
        break;
    case 10:
        switch (net_common_w.err) {
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
            net_common_w.step = 0;
            break;
        }
        if (net_common_w.timer < 271) {
            switch (net_common_w.err) {
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
                net_common_w.step = 0;
            }
        }
        break;
    case 20:
        Ncm_mssage_disp_req(79);
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.step++;
            break;
        }
        if (net_common_w.timer < 271) {
            Ncm_mssage_disp_option_req(79);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.timer = 20;
                net_common_w.step++;
            }
        }
        break;
    case 21:
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.step = 0;
        }
        break;
    case 30:
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.step++;
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
            net_common_w.timer = 20;
            net_common_w.step++;
            Ncm_spr_kill(0x40000);
            Ncm_spr_kill(0x80000);
            Ncm_spr_kill(0x100000);
            break;
        case -1:
            net_common_w.step = 35;
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
        net_common_w.step++;
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
        net_common_w.step = 0;
        break;
    case 40:
        Ncm_mssage_disp_req(6);
        net_common_w.timer--;
        if (net_common_w.timer <= 0) {
            net_common_w.step = 0;
            net_common_w.timer = 0;
            break;
        }
        if (net_common_w.timer < 270) {
            Ncm_mssage_disp_option_req(6);
            if (net_shot_ok_ck(2) != 0) {
                net_common_w.step = 0;
                net_common_w.timer = 0;
            }
        }
        break;
    }
    return ret;
}
