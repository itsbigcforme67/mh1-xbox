/* lb_vs01 - lobby.bin 0x005C49F0-0x005C4CE8: ef_move_sub_005C49F0, the sound-effect
 * script of a village NPC model (per animation id EMW.char0, play footstep /
 * voice requests at animation frames). Same shape as ef_move_sub in lb_em10.
 * Which NPC model this is: not identified (guess: the animation ids 0x263-0x2B6
 * are one model's motions). */
#include "lobby.h"
#include "em.h"

int Code_Make();
void sound_call_005C48C0(EMW *, int, int);
void sound_req_com(EMW *, int, int);
void ashi_sd_req_005C4980(EMW *, f32);

void ef_move_sub_005C49F0(EMW *em) {
    switch (em->char0) {
    case 1:
        break;
    case 2:
        ashi_sd_req_005C4980(em, 20.0f);
        ashi_sd_req_005C4980(em, 54.0f);
        ashi_sd_req_005C4980(em, 88.0f);
        break;
    case 0x40:
        ashi_sd_req_005C4980(em, 26.0f);
        ashi_sd_req_005C4980(em, 60.0f);
        break;
    case 0x263:
        sound_req_com(em, 0x2A, 0x51);
        sound_req_com(em, 0x6C, 0x53);
        break;
    case 0x264:
        sound_req_com(em, 0x7C, 0x4B);
        sound_req_com(em, 0x2A, 0x3D);
        sound_req_com(em, 0x38, 0x52);
        sound_req_com(em, 0x80, 0x53);
        break;
    case 0x28B:
        sound_call_005C48C0(em, 0x14, 0x32);
        break;
    case 0x2A4:
        sound_call_005C48C0(em, 0x18, 0x14);
        sound_call_005C48C0(em, 0x3C, 0x15);
        break;
    case 0x2AB:
        sound_req_com(em, 0x30, 0x37);
        sound_req_com(em, 0x30, 0x35);
        sound_req_com(em, 0x50, 0x37);
        sound_req_com(em, 0x50, 0x35);
        break;
    case 0x2AD:
        sound_req_com(em, 4, 0x4A);
        sound_call_005C48C0(em, 0x24, 0x3D);
        break;
    case 0x2B3:
        sound_req_com(em, 0x2A, 0x31);
        sound_req_com(em, 0x40, 0x31);
        sound_req_com(em, 0xF4, 0x31);
        sound_req_com(em, 0x108, 0x31);
        sound_req_com(em, 0x132, 0x31);
        sound_req_com(em, 0x142, 0x31);
        sound_call_005C48C0(em, 0x50, Code_Make(0x3E, 2, 0x3E, 2));
        sound_call_005C48C0(em, 0x190, Code_Make(0x3F, 2, 0x3F, 1));
        break;
    case 0x2B6:
        sound_req_com(em, 0x22, 0x4B);
        sound_req_com(em, 0x38, 0x4B);
        sound_req_com(em, 0x52, 0x4B);
        break;
    }
}
