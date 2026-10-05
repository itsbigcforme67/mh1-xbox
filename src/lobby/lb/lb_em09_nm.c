/* lb_em09 - lobby.bin 0x0053DD00-0x0053E348. Same for lobby NPC model em09. */
#include "lobby.h"
#include "em.h"

int em_frame_check(EMW *, f32, int);
int em_frame_check3(EMW *, int, f32, f32);
void Npc_se_req();
int Code_Make();
void Eft13_set_em_scl(EMW *, int, f32, int);
void Eft25_set();
void eft01_set();

static void move_default(void) {
}

static void sound_call(EMW *em, int frame, int se) {
    if (em_frame_check(em, (f32)frame, 0) != 0) {
        Npc_se_req(em, se, (u8 *)em + 0xAC, 2);
    }
}

static void ef_move_sub(EMW *em) {
    switch (em->char0) {
    case 0x3E9:
        break;
    case 0x3EA:
        sound_call(em, 0x12, 0xB);
        sound_call(em, 0x26, 0xB);
        sound_call(em, 0x72, 0xB);
        sound_call(em, 0x7E, 0xB);
        sound_call(em, 0x98, 0xB);
        sound_call(em, 8, 8);
        sound_call(em, 0x58, 8);
        sound_call(em, 0xD0, 8);
        return;
    case 0x3EB:
        sound_call(em, 0x26, 0xA);
        sound_call(em, 6, 8);
        sound_call(em, 0x16, 8);
        sound_call(em, 0x9A, 8);
        sound_call(em, 0xA4, 8);
        return;
    case 0x3EC:
        sound_call(em, 0x4C, Code_Make(0xD, 1, 0xD, 2));
        sound_call(em, 8, 8);
        sound_call(em, 0x50, 8);
        sound_call(em, 0x8C, 8);
        sound_call(em, 0xC8, 8);
        return;
    case 0x3ED:
        sound_call(em, 6, 0x10);
        return;
    case 0x3F3:
        sound_call(em, 0x50, Code_Make(0xA, 2, 0xA, 2));
        sound_call(em, 0xC, 8);
        sound_call(em, 0x1E, 8);
        sound_call(em, 0x2E, 8);
        sound_call(em, 0x42, 8);
        sound_call(em, 0x58, 8);
        sound_call(em, 0x76, 8);
        sound_call(em, 0x8C, 8);
        sound_call(em, 0x98, 8);
        sound_call(em, 0xA2, 8);
        sound_call(em, 0xAE, 8);
        sound_call(em, 0xB8, 8);
        sound_call(em, 0xC2, 8);
        sound_call(em, 0xAE, 8);
        return;
    case 0x3F4:
        sound_call(em, 8, 0x12);
        sound_call(em, 0xA, 8);
        sound_call(em, 0x16, 8);
        sound_call(em, 0x20, 8);
        sound_call(em, 0x2E, 8);
        sound_call(em, 0x38, 8);
        sound_call(em, 0x44, 8);
        sound_call(em, 0x4E, 8);
        sound_call(em, 0x58, 8);
        sound_call(em, 0x64, 8);
        sound_call(em, 0x70, 8);
        return;
    case 0x3F8:
        sound_call(em, 0xA, 8);
        sound_call(em, 0xA, 0x13);
        sound_call(em, 0x18, 8);
        sound_call(em, 0x26, 8);
        sound_call(em, 0x32, 8);
        return;
    case 0x42E:
        sound_call(em, 4, 0x10);
        sound_call(em, 0x100, 0xB);
        return;
    case 0x42F:
        sound_call(em, 0x10, 8);
        sound_call(em, 0x12, 8);
        sound_call(em, 0x24, 0x10);
        return;
    case 0x430:
        sound_call(em, 0x30, 0xC);
        sound_call(em, 8, 0x50);
        return;
    case 0x431:
        sound_call(em, 0xA, 0xA);
        return;
    case 0x432:
        sound_call(em, 8, 0xE);
        sound_call(em, 0x34, 9);
        sound_call(em, 0x44, 0x11);
        sound_call(em, 0x16, 8);
        sound_call(em, 0x1E, 8);
        sound_call(em, 0x80, 8);
        sound_call(em, 0x94, 8);
        return;
    case 0x433:
        sound_call(em, 0xE, 0xF);
        return;
    default:
        move_default();
        break;
    }
}

void em09_effect_move(EMW *em) {
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

void em09_local_init(EMW *em) {
    eft01_set(em, 0);
}

void dummy_em_prog(void) {
}
