/* lbem04, run 1: move_default_0053E350 .. move_default_0053E350 (lobby.bin 0x0053E350-0x0053E358): the matching functions of lb_em04_nm.c. */
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
