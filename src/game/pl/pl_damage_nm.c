/* pl_damage near-match file, kept for reference: pl_guard_ck now matches (see pl_guard.c). */
#include "pl.h"

s16 act_ck(PLW *, int, int);
void adx_se_stop(PLW *);
void pl_flag_clr(PLW *, int);
void pl_flag_set(PLW *, int);
int pl_flag_ck(PLW *, int);
void Pl_view_reset(PLW *);
void Pl_act_set(PLW *, int, int, int);
void Pl_stamina_calc(PLW *, int);
void Pl_slash_calc(PLW *, int);
int Pl_master_ck(PLW *);
void Pl_vital_calc(PLW *, s16);
int Pl_Skill_ck(PLW *, int);
void Pl_poison_add(PLW *, s16);
unsigned char Pl_piyo_ck(PLW *);
void vib_set_pl(PLW *, int);
int Code_Make(int, int, int, int);
void Pl_se_req2(PLW *, int, int, f32 *, int, int);

int Guard_dir_ck(u16 ang, u16 dm_ang);

u8 pl_guard_ck(PLW *pl) {
    u8 ret = 0;

    if (pl->stamina < 0x4B) {
        return 0;
    }
#ifdef VERSION_MHG
    /* G pl_guard_ck (game 0x66CF50): a hit whose attack data has bits 0xC0 (PLW+0x3EB) is guarded only with skill 169 */
    if ((*((u8 *)pl + 0x3EB) & 0xC0) && Pl_Skill_ck(pl, 169) == 0) {
        return 0;
    }
#endif
    switch (pl->kind) {
    case 0:
    case 4:
    case 3:
        if (pl->flag12 != 0) {
            switch (pl->st) {
            case 0:
            case 1:
#ifdef VERSION_MHG
                /* G: with skill 79 and PLW+0x940 set, the guard flag is not needed (only the direction) */
                if (((Pl_Skill_ck(pl, 79) == 1 && *((u8 *)pl + 0x940) != 0) || pl_flag_ck(pl, 0x8000) != 0)
                    && Guard_dir_ck(pl->ang[1], pl->dm_ang) == 1) {
                    ret = 1;
                }
#else
                if (pl_flag_ck(pl, 0x8000) != 0 && Guard_dir_ck(pl->ang[1], pl->dm_ang) == 1) {
                    ret = 1;
                }
#endif
                break;
            }
        }
        break;
    }
    return ret;
}

