/* pl_damage near-match, NOT built: pl_guard_ck (0x0063A010).
 * Player damage: death, guarding (direction check, stamina cost, recoil)
 * One instruction off: our compiler masks the u8 result at the return
 * (andi v0, s0, 0xFF); the original returns it unmasked and Pl_damage_sub
 * masks it after the call. */
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
int Pl_piyo_ck(PLW *);
void vib_set_pl(PLW *, int);
int Code_Make(int, int, int, int);
void Pl_se_req2(PLW *, int, int, f32 *, int, int);

int Guard_dir_ck(u16 ang, u16 dm_ang);

int pl_guard_ck(PLW *pl) {
    u8 ret = 0;

    if (pl->stamina < 0x4B) {
        return 0;
    }
    switch (pl->kind) {
    case 0:
    case 4:
    case 3:
        if (pl->flag12 != 0) {
            switch (pl->st) {
            case 0:
            case 1:
                if (pl_flag_ck(pl, 0x8000) != 0 && Guard_dir_ck(pl->ang[1], pl->dm_ang) == 1) {
                    ret = 1;
                }
                break;
            }
        }
        break;
    }
    return ret;
}

