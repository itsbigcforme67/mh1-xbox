/* lb_em04 - lobby.bin 0x0053E350-0x0053E848. Same for lobby NPC model em04. */
#include "lobby.h"
#include "em.h"

typedef struct { u8 pad[0x2A]; s8 eff; } LBEFW;

int em_frame_check(EMW *, f32, int);
int em_frame_check3(EMW *, int, f32, f32);
void Npc_se_req();
int Code_Make();
void Eft13_set_em_scl(EMW *, int, f32, int);
void Eft25_set();
void eft01_set();
void sound_call_0053E7E0(EMW *em, int frame, int se);

void move_default_0053E350(void) {
}

void ef_move_sub_0053E360(EMW *em, LBEFW *w) {
    int c = em->char0;
    switch (c) {
    case 0x3E9:
        sound_call_0053E7E0(em, 0x10, Code_Make(3, 1, 0, 1));
        sound_call_0053E7E0(em, 0x4C, Code_Make(3, 1, 0, 1));
        return;
    case 0x3EA:
        sound_call_0053E7E0(em, 0xC, Code_Make(3, 2, 3, 2));
        sound_call_0053E7E0(em, 6, 1);
        sound_call_0053E7E0(em, 0x10, 1);
        sound_call_0053E7E0(em, 0x24, 1);
        return;
    case 0x3EB:
        sound_call_0053E7E0(em, 0xC, Code_Make(3, 2, 3, 2));
        sound_call_0053E7E0(em, 0x26, Code_Make(3, 2, 3, 2));
        sound_call_0053E7E0(em, 6, 2);
        sound_call_0053E7E0(em, 0x1E, 2);
        sound_call_0053E7E0(em, 0x2C, 2);
        return;
    case 0x3F2:
        sound_call_0053E7E0(em, 2, Code_Make(4, 6, 3, 2));
        sound_call_0053E7E0(em, 0x40, Code_Make(4, 6, 3, 2));
        sound_call_0053E7E0(em, 0x60, Code_Make(3, 6, 4, 2));
        sound_call_0053E7E0(em, 0x80, Code_Make(4, 6, 3, 2));
        sound_call_0053E7E0(em, 0xA0, Code_Make(3, 6, 4, 2));
        sound_call_0053E7E0(em, 0x16, 1);
        sound_call_0053E7E0(em, 0x74, 1);
        return;
    case 0x3F3:
        sound_call_0053E7E0(em, 0x16, 4);
        sound_call_0053E7E0(em, 0xA, 1);
        sound_call_0053E7E0(em, 0x52, 1);
        return;
    case 0x3F6:
        sound_call_0053E7E0(em, 0x1E, 6);
        sound_call_0053E7E0(em, 0x16, 6);
        if (em_frame_check(em, 66.0f, 0) != 0) {
            Eft13_set_em_scl(em, 6, 0.7f, 3);
        }
        break;
    case 0x3F7:
        sound_call_0053E7E0(em, 0xA, 6);
        sound_call_0053E7E0(em, 0x18, 2);
        sound_call_0053E7E0(em, 0x1C, 2);
        sound_call_0053E7E0(em, 0x24, 2);
        if (em_frame_check(em, 16.0f, 0) != 0) {
            Eft13_set_em_scl(em, 4, 2.0f, 3);
        }
        if (em_frame_check(em, 30.0f, 0) != 0) {
            Eft13_set_em_scl(em, 0x11, 3.5f, 3);
        }
        break;
    case 0x3FA:
        sound_call_0053E7E0(em, 0x14, 6);
        sound_call_0053E7E0(em, 0x18, 5);
        sound_call_0053E7E0(em, 0x2A, 2);
        sound_call_0053E7E0(em, 0x2E, 2);
        return;
    default:
        move_default_0053E350();
        break;
    }
}

void em04_effect_move_0053E790(EMW *em) {
    LBEFW *w = (LBEFW *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_0053E360(em, w);
        break;
    }
}

void sound_call_0053E7E0(EMW *em, int frame, int se) {
    if (em_frame_check(em, (f32)frame, 0) != 0) {
        Npc_se_req(em, se, (u8 *)em + 0xAC, 2);
    }
}

void dummy_em_prog_0053E840(void) {
}
