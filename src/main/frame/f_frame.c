/* Motion system, matching part 1 (SLPM_654.95 0x00125340-0x00125730):
 * create_plcom_motion, create_pl_motion, create_em_motion. See f_frame_nm.c
 * for the whole file and what the motion system does. */
#include "types.h"
#include "game.h"
#include "pl.h"
#include "em.h"
#include "frame.h"

void create_plcom_motion(void) {
    u8 tmp[0x28];
    u8 frame[8];
    int idx;
    u8 *aan;
    int num;
    s32 *ofs;
    int i;
    u8 *p;
    u32 *han;
    u32 h;
    int base;
    int bank;

    idx = 0;
    han = motion_set_handle_tbl;
    ofs = com_mot_han_ofs;
    bank = 0;
    aan = pl_area_top;
    base = 0;
    for (; bank < 10; bank++, ofs++, base += 100) {
        *ofs = idx;
        num = aan_ctr_get(aan, bank);
        for (i = 0; i < num; i++, han++, idx++) {
            p = aan_ofs_calc(aan, i + base);
            if (p != 0) {
                flGetFrame(frame);
                plCreateMotionSetFromAAN(tmp, data_load_ptr, p);
                h = flCreateMotionSetHandle(tmp);
                flReleaseFrame(frame);
                *han = h;
            } else {
                *han = 0;
            }
        }
    }
}

void create_pl_motion(int pl) {
    u8 tmp[0x28];
    u8 frame[8];
    int idx;
    u8 *aan;
    int num;
    s32 *ofs;
    int i;
    u8 *p;
    u32 *han;
    u32 h;
    int base;
    int bank;

    han = &motion_set_handle_tbl[pl * 300 + 500];
    aan = pl_area_top;
    idx = 0;
    bank = 0;
    ofs = pl_mot_han_ofs[pl];
    base = 0;
    for (; bank < 6; bank++, ofs++, base += 100) {
        *ofs = idx;
        num = aan_ctr_get(aan, bank);
        for (i = 0; i < num; i++, han++, idx++) {
            p = aan_ofs_calc(aan, i + base);
            if (p != 0) {
                flGetFrame(frame);
                plCreateMotionSetFromAAN(tmp, data_load_ptr, p);
                h = flCreateMotionSetHandle(tmp);
                flReleaseFrame(frame);
                *han = h;
            } else {
                *han = 0;
            }
        }
    }
}

void create_em_motion(int no, int em) {
    u8 *aan;
    u8 tmp[0x28];
    u8 frame[8];
    int bank;
    int idx;
    int num;
    int i;
    u8 *p;
    u32 *han;
    u32 h;
    int banks;

    han = &motion_set_handle_tbl[no * 600 + 1700];
    aan = pl_area_top;
    banks = Em_max_parts_get(em) * 2;
    idx = 0;
    for (bank = 0; bank < banks; bank++) {
        em_mot_han_ofs[no][bank] = idx;
        num = aan_ctr_get(aan, bank);
        for (i = 0; i < num; i++, han++, idx++) {
            p = aan_ofs_calc(aan, i + bank * 100);
            if (p != 0) {
                flGetFrame(frame);
                plCreateMotionSetFromAAN(tmp, data_load_ptr, p);
                h = flCreateMotionSetHandle(tmp);
                flReleaseFrame(frame);
                *han = h;
            } else {
                *han = 0;
            }
        }
    }
}
