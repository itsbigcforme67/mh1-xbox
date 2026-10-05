/* lb_em10 - lobby.bin 0x0053D7D0-0x0053DCF8. Sound-effect script of lobby NPC
 * model em10: per animation id (EMW.char0) play Npc_se_req at animation
 * frames (sound_call). Near-match from m2c. */
#include "lobby.h"
#include "em.h"

int em_frame_check(EMW *, f32, int);
int em_frame_check3(EMW *, int, f32, f32);
void Npc_se_req();
int Code_Make();
void Eft13_set_em_scl(EMW *, int, f32, int);
void Eft25_set();
void eft01_set();

static void sound_call(EMW *em, int frame, int se) {
    if (em_frame_check(em, (f32)frame, 0) != 0) {
        Npc_se_req(em, se, (u8 *)em + 0xAC, 2);
    }
}

static void ef_move_sub(EMW *em) {
    switch (em->char0) {
    case 0x3EF:
        sound_call(em, 0xA, 0x26);
        return;
    case 0x3F0:
        sound_call(em, 0x2A, 0x27);
        sound_call(em, 0xEE, 0x28);
        sound_call(em, 0x15E, 0x29);
        return;
    case 0x3F5:
        sound_call(em, 0x26, Code_Make(0x2A, 1, 0x2B, 1));
        sound_call(em, 0x6C, Code_Make(0x2C, 1, 0x2A, 1));
        sound_call(em, 0xC, 0x24);
        sound_call(em, 0x2C, 0x25);
        sound_call(em, 0x4C, 0x24);
        sound_call(em, 0x6A, 0x25);
        return;
    case 0x3F7:
        sound_call(em, 0x42, 0x2D);
        sound_call(em, 0xC2, 0x2E);
        return;
    case 0x3F8:
        sound_call(em, 0xE, 0x30);
        return;
    case 0x3FA:
        sound_call(em, 0xE, 0x2F);
        sound_call(em, 0x8C, 0x27);
        return;
    case 0x3FB:
        sound_call(em, 4, 0x29);
        sound_call(em, 0x34, 0x24);
        sound_call(em, 0x52, 0x24);
        sound_call(em, 0x94, 0x31);
        return;
    case 0x3FC:
        sound_call(em, 4, 0x26);
        sound_call(em, 0x3A, 0x2F);
        sound_call(em, 0x5E, 0x24);
        sound_call(em, 0x78, 0x30);
        return;
    case 0x3FD:
        sound_call(em, 8, 0x25);
        sound_call(em, 0x10, 0x25);
        sound_call(em, 0x1A, 0x25);
        sound_call(em, 0x26, 0x25);
        return;
    case 0x3FE:
        sound_call(em, 8, 0x25);
        sound_call(em, 0x12, 0x25);
        sound_call(em, 0x1C, 0x25);
        sound_call(em, 0x26, 0x25);
        sound_call(em, 0x32, 0x25);
        sound_call(em, 0x3C, 0x24);
        sound_call(em, 0x38, 0x2B);
        sound_call(em, 0x82, 0x2C);
        sound_call(em, 0xE6, 0x4D);
        return;
    case 0x401:
        sound_call(em, 0x32, 0x2F);
        sound_call(em, 0xAA, 0x2B);
        sound_call(em, 0xE2, 0x28);
        return;
    case 0x403:
        sound_call(em, 0xE, 0x2F);
        sound_call(em, 0x8C, 0x27);
        sound_call(em, 0x162, 0x2C);
        return;
    case 0x400:
        if (em_frame_check(em, 84.0f, 0) != 0) {
            Eft13_set_em_scl(em, 0xD, 1.0f, 0x1C);
        }
        if (em_frame_check3(em, 0, 38.0f, 198.0f) != 0 && *(u16 *)0x3F340E % 10 == 0) {
            Eft25_set(em, 2);
        }
    }
}

void em10_effect_move(EMW *em) {
    u8 *ex;
    s8 step = *(s8 *)((u8 *)em + 0x46E);

    ex = em->ex;

    switch (step) {
    case 0:
        ex[0x2A] = step + 1;
        break;
    case 1:
        ef_move_sub(em);
        break;
    }
}

void em10_local_init(EMW *em) {
    eft01_set(em, 0);
}

void dummy_em_prog(void) {
}
