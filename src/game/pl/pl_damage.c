/* pl_damage - game.bin 0x00639EB0-0x0063A008: mahi_dm_ck to Guard_dir_ck.
 * Player damage: death, guarding (direction check, stamina cost, recoil)
 * and the reaction when hit. pl_guard_ck (next) is still assembly (near-match
 * in pl_damage_nm.c); the rest is in pl_damageb.c. */
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

int mahi_dm_ck(PLW *pl) {
    if (act_ck(pl, 2, 0x16) != 0 || act_ck(pl, 2, 0x18) != 0) {
        return 1;
    }
    return 0;
}

void Pl_die_set(PLW *pl);
void Pl_dm_clear(PLW *pl);

void Pl_die_set(PLW *pl) {
    if ((u8)pl->flag14 != 3) {
        adx_se_stop(pl);
        pl_flag_clr(pl, 2);
        pl_flag_clr(pl, 0x8000);
        pl->vital = 0;
        pl->vital_red = 0;
        Pl_view_reset(pl);
        pl->x7AC = 0;
        pl->x7AA = 0;
        pl->x7BA = 0;
        pl->x7B2 = 0;
        pl->x7C4 = 0;
        pl->x738 = 0;
        Pl_act_set(pl, 3, 0, 0);
    }
}

void Pl_dm_clear(PLW *pl) {
    pl->x610 = 0;
    pl->x40A = 0;
}

int Guard_dir_ck(u16 ang, u16 dm_ang) {
    u16 d = (u16)(ang - dm_ang) + 0x4000;

    if (d <= 0xE39 || d >= 0x71C7) {
        return 1;
    }
    return 0;
}
