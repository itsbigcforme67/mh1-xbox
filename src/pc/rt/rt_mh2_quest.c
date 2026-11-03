/*
 * rt_mh2_quest.c - MH Dos's own quest flow on the PC (VERSION_MH2, docs/mh2/quest_flow.md).
 *
 * MH2's game task (sub_main 0x326E64) calls 0x3A2E30 where G's game2 calls Quest_condition_judging: the same state machine
 * (quest_w.x00 / x01) with MH2's changes, and 0x3A9230, MH2's quest script (G's quest_condition_prog with other opcodes).
 * G's judging read MH2's script with G's opcodes and "cleared" quest 1014 at tick 7. Here both run on the PC as C,
 * written from CODE's near-match C (src/mh2/sub_main/p003A2E30_nm.c, p003A9230_nm.c: 96 and 11 instructions off) and from
 * our own disassembly for their callees. Not byte-checked: this file is host C (raw MH2 offsets on the host's quest_w,
 * game_w and player_work, which keep MH2's layout), a stand-in until CODE has matched C for each function.
 *
 * What MH2 calls that the host does not do (each said at its place): net_send_sys (offline: never sends), game 0x6EE380
 * (an on-screen notice, not read), the timed pick points 0x3A5880 (the host's StiEM_data is G's 0x1C-byte form, MH2 has a
 * timer at +0x1E: skipped), system_w+0x33 (a byte the host's G-form system_w uses otherwise: kept in this file).
 *
 * The quest targets (MH2, new): quest_w+0x160, 3 live entries and their copies at +0x178, 8 bytes each: {s32 flags; u16 id;
 * s16 count}. Flags 0x10 in use, 0x20 done, 0x40 a "must not" target (0x400 when it failed), 0x02 deliver items (0x80: the
 * master's box only), 0x4000 a bit of quest_w+0x48C, 0x8000 damage to the boss, 0x10000 only after half the time, 0x100 a
 * label to jump to (quest_w+0x0B) when done. The meanings come from the code that tests them; the names are ours.
 */
#ifdef VERSION_MH2
#include "rt.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern u8 quest_w[], game_w[], player_work[];
extern u8 *mission_area;

int Online_ck();
int set01_set2();
int set01_set();
int set01_set2_use_mem();
unsigned char Get_hunter_rank();
int Pl_item_num_ck();
s16 act_ck();
int adx_se_set();
int se_req();
int ran_suu();
int Bdora_hp_ck();
extern u8 User_data[];

/* quest_w and game_w by MH2 offset (sub_main 0x5D8260 and 0x6ADCB0) */
#define QS8(o) (*(s8 *)(quest_w + (o)))
#define QU8(o) (*(u8 *)(quest_w + (o)))
#define QS16(o) (*(s16 *)(quest_w + (o)))
#define QU16(o) (*(u16 *)(quest_w + (o)))
#define QS32(o) (*(s32 *)(quest_w + (o)))
#define QPTR(t, o) (*(t *)(quest_w + (o)))
#define GU8(o) (*(u8 *)(game_w + (o)))
#define G_MASTER GU8(0x3C)
#define G_PLNUM GU8(0x3E)
#define G_STATE GU8(0x40)           /* G's x0D5: 2 running, 3 cleared, 4 done, 5 failed, 6 abandoned, 7 / 8 errors */
#define G_PLSTATE(i) GU8(0x224 + (i))
#define PLW_SIZE 0x970
#define PL(i) (player_work + (i) * PLW_SIZE)

#define Q_STEP QS8(0x00)
#define Q_SUB QS8(0x01)
#define Q_END QS8(0x06)
#define Q_NO QU16(0x08)
#define Q_TIMER QS32(0x10)
#define Q_PC QS16(0x1E)
#define Q_FLAGS QS32(0x28)
#define Q_CMD QPTR(QCMD *, 0x54)    /* the script (mission header +0x10) */
#define Q_MSGS QPTR(s32 *, 0x58)    /* offsets of the script's messages in the mission file (header +0x30) */
#define Q_INFO QPTR(u8 *, 0x7C)     /* the info block (time at +0x1C, targets at +0x2C, target mode +0x44) */
#define Q_BOSS QPTR(u8 *, 0x98)     /* the monster the damage targets watch */
#define Q_TGT_MODE QU8(0x154)       /* 1 any target, 2 the first, 3 the first two, 4 all */

typedef struct { s16 op, a, b, c; } QCMD;
#define TF(n) QS32(0x160 + 8 * (n))
#define TID(n) QU16(0x164 + 8 * (n))
#define TCNT(n) QS16(0x166 + 8 * (n))

static u8 sysw33;   /* system_w+0x33 on the PS2 (0x81 when the quest clears; 0x80 is set by code the PC does not run) */
static int trace;

/* a word of PS2 data (a pointer the import may have made a host pointer) as a host pointer */
static void *ps2_ptr(uint32_t va)
{
    const uint8_t *p = rt_addr(va, 4);
    uint32_t w;
    if (!p)
        return NULL;
    memcpy(&w, p, 4);
    if (w >= 0x02000000u && rt_va_of_host((const void *)(uintptr_t)w))
        return (void *)(uintptr_t)w;
    return w ? (void *)rt_addr(w, 1) : NULL;
}
static void msg(uint32_t va)        /* set01_set2 of a message word in sub_main data */
{
    void *s = ps2_ptr(va);
    if (s)
        set01_set2(s);
}
/* set01_set(type, n, arg): the host's set01 is G's, with info_strT tables; where MH2's table is not placed (info_str0) or is too
 * short for n (info_str3 has 3 words in the PC's symbol view, MH2 asks for 3), the message is left out instead of reading a NULL */
void *rt_host_sym(const char *n);
size_t rt_table_size(const char *name);
static void info_set(int type, int n, int arg)
{
    char nm[16];
    char **t;
    snprintf(nm, sizeof nm, "info_str%d", type);
    t = rt_host_sym(nm);
    if (!t || (size_t)n >= rt_table_size(nm) / 4 || !t[n]) {
        rt_log_standin("MH2 set01_set: a message whose info_str table is not placed (left out)");
        if (trace)
            fprintf(stderr, "rt_mh2_quest: set01_set(%d, %d, %d) left out: %s[%d] not placed\n", type, n, arg, nm, n);
        return;
    }
    set01_set(type, n, arg);
}
/* net_send_sys (gm_sub 0x901790): the PC is offline, nothing to send. The fields MH2 fills first (quest_w+0x551..) are filled. */
static void net_send(void) {}
/* game 0x6EE380(n) (0x2B0 bytes, n = 14, 15, 17, 18): a notice on screen, not read yet */
static void notice(int n)
{
    static int said;
    if (!said++)
        rt_log_standin("MH2 game 0x6EE380 (quest notice) not ported");
    if (trace)
        fprintf(stderr, "rt_mh2_quest: notice %d (game 0x6EE380, not ported)\n", n);
}
static void net_fields(int kind, int a, int b)
{
    QS16(0x552) = 0;
    QS8(0x551) = (s8)kind;
    QS16(0x554) = (s16)a;
    QS16(0x556) = (s16)b;
}

/* 0x39E400: an online-only quest (25001) in an online session */
static int online_quest(void) { return Q_NO == 25001 && Online_ck() == 1; }
static int qflag_c000000(void) { return (Q_FLAGS & 0x0C000000) != 0; }   /* 0x39F990 */
static int qflag_4000000(void) { return (Q_FLAGS & 0x04000000) != 0; }   /* 0x39F970 */
static int qflag_400000(void) { return (Q_FLAGS & 0x00400000) != 0; }    /* 0x39F9D0 */
/* 0x36BA50: game_w+0x41 == 1 (online, the guild's check juchu_chk; not on the PC) else quest flag 0x200000 */
static int qflag_200000(void)
{
    if (GU8(0x41) == 1)
        return 0;
    return (Q_FLAGS & 0x00200000) != 0;
}
/* 0x3B5A40 */
static int same_side(int a, int b, int c)
{
    a &= 0xFF;
    b &= 0xFF;
    if (a == 0 || b == 0)
        return (c & 0xFF) == 0;
    return ((a & 1) ^ (b & 1)) == 0;
}

/* Quest_clear_ck 0x3A1F60 (MH2's: G's host copy reads the same byte but is not used here) */
static int clear_ck(int a)
{
    int s = G_STATE;
    if (s == 3 || s == 4)
        return 1;
    if ((s == 5 || s == 6 || s == 7) && a)
        return -1;
    return 0;
}
/* Quest_time_get 0x3A1FC0: 1 the quest's time (info+0x1C), else the time left. G's copy (f_quest.c, renamed away for MH2 by
 * tools/build_pc.sh) read the time at G's info+0x10, which in MH2 is the fee: Quest_timer_reset set 800 ticks for quest 1014. */
int Quest_time_get(int a)
{
    if (a == 1)
        return Q_INFO ? *(s32 *)(Q_INFO + 0x1C) : 0;
    return Q_TIMER >= 0 ? Q_TIMER : 0;
}
/* Game_clear_ck 0x36BB10: G's cases, on MH2's game_w+0x40 (dsp01.c reads G's +0x3E: renamed away for MH2 by tools/build_pc.sh) */
int Game_clear_ck(int a)
{
    switch (G_STATE) {
    case 3: return a == 0;
    case 5: return a != 2;
    case 4: case 6: case 7: case 8: return 1;
    }
    return 0;
}
/* Quest_error_set2 0x3A1CA0, Quest_error_set 0x3A1CD0 */
static void error_set(void)
{
    if (clear_ck(1) == 0) {
        G_STATE = 8;
        Q_END = 8;
        Q_PC = -1;
    }
}
static s16 find_op(int key)
{
    QCMD *c = Q_CMD;
    s16 i = 0;
    while (c[i].op != key)
        i++;
    return i;
}
/* quest_failed_ptr_set 0x3AAD90: the script jumps to its "failed" part (op -2, 37 with a) */
static void failed_ptr_set(int a)
{
    G_STATE = 5;
    Q_PC = find_op(a ? 37 : -2);
}
/* 0x3AADF0: while the quest runs, the script jumps to its "cleared" part (op -1, 38 with a) */
static void clear_ptr_set(int a)
{
    if (G_STATE > 2)
        return;
    Q_PC = find_op(a ? 38 : -1);
    G_STATE = 3;
}

/* ------------------------------------------------ targets */
/* 0x3A7FC0 and 0x3A80F0: clear the six entries; add one in the first free slot (and its copy) */
static void tgt_clear(void) { memset(quest_w + 0x160, 0, 0x30); }
static void tgt_add(int flags, int id, int cnt)
{
    int k;
    for (k = 0; k < 3 && TF(k) != 0; k++)
        ;
    if (k >= 3)
        return;
    TF(k) = TF(k + 3) = flags | 0x10;
    TID(k) = TID(k + 3) = (u16)id;
    TCNT(k) = TCNT(k + 3) = (s16)cnt;
}
/* 0x3A87D0: target n done (a "must not" target never counts) */
static int tgt_done(int n)
{
    s32 f = TF(n);
    if (!(f & 0x10) || (f & 0x440) == 0x40)
        return 0;
    return (f & 0x20) != 0;
}
/* 0x3A84C0(mode): are the targets met by the quest's mode? A "must not" target that failed fails the quest (mode 1). */
static int tgt_met(int mode)
{
    int k, done = 0, unused = 0;
    for (k = 0; k < 3; k++) {
        s32 f = TF(k);
        if (f & 0x10) {
            if (f & 0x20)
                done |= 1 << k;
            if ((f & 0x440) == 0x40) {
                TF(k) = f | 0x400;
                if (mode == 1)
                    failed_ptr_set(0);
                return 0;
            }
        } else
            unused |= 1 << k;
    }
    if (online_quest() == 1)
        return 0;
    switch (Q_TGT_MODE) {
    case 1: if (!done) return 0; break;
    case 2: if ((done & 1) != 1) return 0; break;
    case 3: if ((done & 3) != 3) return 0; break;
    case 4: if ((done | unused) != 7) return 0; break;
    default: return 0;
    }
    if (mode == 1)
        clear_ptr_set(0);
    return 1;
}
/* 0x3A8680: a secondary target done (modes 2 and 3), the same failure test as above */
static int tgt_sub_met(void)
{
    int k, done = 0;
    for (k = 0; k < 3; k++) {
        s32 f = TF(k);
        if (!(f & 0x10))
            continue;
        if (f & 0x20)
            done |= 1 << k;
        if ((f & 0x440) == 0x40) {
            TF(k) = f | 0x400;
            failed_ptr_set(0);
            return 0;
        }
    }
    switch (Q_TGT_MODE) {
    case 3: return (done & 4) != 0;
    case 2: return (done & 6) != 0;
    }
    return 0;
}
/* 0x3A89C0: the count of the live target with this id (-1 none) */
static int tgt_count(int id)
{
    int k;
    for (k = 0; k < 3; k++)
        if ((TF(k) & 0x11) == 0x11 && TID(k) == (u16)id)
            return TCNT(k);
    return -1;
}
/* 0x3A8F60: a done target that asks for a jump (flag 0x100) */
static int tgt_jump(void)
{
    int k;
    for (k = 0; k < 3; k++)
        if ((TF(k) & 0x100) && (TF(k) & 0x31) == 0x31)
            return 1;
    return 0;
}
/* 0x3A8AB0, 0x3A8B70: items asked (from the copy) and items delivered (the delivery box, PLW+0x84C: 4 x {u16 id, s16 n}) */
static int tgt_asked(int id, int n)
{
    if ((TF(n + 3) & 0x12) == 0x12 && TID(n + 3) == (u16)id)
        return TCNT(n + 3);
    return -1;
}
static int tgt_given(int id)
{
    int k, i, j, sum = 0;
    for (k = 0; k < 3; k++)
        if ((TF(k) & 0x12) == 0x12 && TID(k) == (u16)id)
            break;
    if (k >= 3)
        return 0;
    if (TF(k) & 0x80) {
        u8 *pl = PL(G_MASTER);
        for (j = 0; j < 4; j++)
            if (*(u16 *)(pl + 0x84C + 4 * j) == (u16)id)
                return *(s16 *)(pl + 0x84E + 4 * j);
        return 0;
    }
    for (i = 0; i < G_PLNUM; i++)
        for (j = 0; j < 4; j++)
            if (*(u16 *)(PL(i) + 0x84C + 4 * j) == (u16)id)
                sum += *(s16 *)(PL(i) + 0x84E + 4 * j);
    return (s16)sum;
}
/* 0x3A81A0(n, why): target n is done: its message, a notice for some kinds, the sound, then tell the others */
static void tgt_set_done(int n, int why)
{
    int k;
    n &= 0xFFFF;
    if (tgt_done(n) == 1)
        return;
    TF(n) |= 0x20;
    if (trace)
        fprintf(stderr, "rt_mh2_quest: target %d done (0x%X)\n", n, why);
    if (online_quest() == 1)
        return;
    for (k = 0; k < 3; k++)
        if (QU8(0x541 + k) == 0xFF) {
            QU8(0x541 + k) = (u8)n;
            break;
        }
    if (why == 0x200)
        notice(17);
    if (why == 0x100)
        notice(18);
    if (why == 2)
        notice(15);
    switch (Q_TGT_MODE) {
    case 1:
        msg(0x445590);
        break;
    case 2:
        if (qflag_400000() == 1)
            msg(0x445590);
        else
            msg(0x445584 + 4 * n);
        break;
    case 3:
    case 4:
        msg(tgt_met(0) == 1 ? 0x445590 : 0x445594);
        break;
    }
    if (tgt_met(0) == 0) {
        if (n != 0) {
            int a = (n - 1) * 8 + 24, e = a + 8;
            for (; a < e; a++)
                if (*(u16 *)(game_w + 0x140 + 4 * a) != 0) {
                    if (qflag_400000() == 1)
                        msg(0x44556C);
                    else
                        msg(n == 1 ? 0x44557C : 0x445580);
                    break;
                }
        }
        adx_se_set(183);
    }
    QU16(0x558) = (u16)n;
    net_fields(16, why >> 16, why);
    net_send();
}
/* 0x3A8470 */
static void tgt_set(int n, int why)
{
    if (clear_ck(1) == 0)
        tgt_set_done(n, why);
}
/* 0x3A8CB0: every tick, the item, flag and damage targets */
static void tgt_tick(void)
{
    int k;
    for (k = 0; k < 3; k++)
        if ((TF(k) & 0x32) == 0x12) {
            int a = tgt_asked(TID(k), k);
            TCNT(k) = (s16)(a - tgt_given(TID(k)));
            if (TCNT(k) <= 0)
                tgt_set(k, 2);
        }
    for (k = 0; k < 3; k++)
        if ((TF(k) & 0x4034) == 0x4014 && (QU16(0x48C + 2 * TID(k)) & (1 << TCNT(k))))   /* 0x3A2BB0 */
            tgt_set(k, 0x4000);
    for (k = 0; k < 3; k++) {
        s32 f = TF(k);
        u8 *w = Q_BOSS;
        if ((f & 0x8034) != 0x8014 || !w)
            continue;
        if ((f & 0x10000) && Quest_time_get(1) / 2 < Quest_time_get(0))
            continue;
        if (QS16(0x134) - *(s16 *)(w + 0x2C2) < TCNT(k) * 100)
            continue;
        if (f & 0x10000)
            msg(0x4455E0);
        tgt_set(k, 0x8000);
    }
}
/* 0x3A8060 (from Quest_start): the target mode and up to three targets from the info block (+0x44, +0x2C) */
static void tgt_init(void)
{
    u8 *info = Q_INFO, *t = info + 0x2C;
    int k;
    Q_TGT_MODE = info[0x44];
    for (k = 0; k < 3; k++, t += 8) {
        if (*(s32 *)t)
            tgt_add(*(s32 *)t, *(s16 *)(t + 4), *(s16 *)(t + 6));
        QU8(0x541 + k) = 0xFF;
    }
}

/* ------------------------------------------------ the item checks quest_w+0x1B0 (two of {s32 state; u16 item; s8 n; s8 v}) */
static void items_clear(void) { memset(quest_w + 0x1B0, 0, 16); }      /* 0x3A3DF0 (it clears 7 of each 8 bytes) */
/* 0x3A3E20: from the mission file (+100) */
static void items_init(void)
{
    u8 *s = QPTR(u8 *, 0x4C) + 100, *d = quest_w + 0x1B0;
    int k;
    for (k = 0; k < 2; k++, s += 8, d += 8)
        if ((*(s32 *)d = *(s32 *)s) != 0) {
            *(u16 *)(d + 4) = *(u16 *)(s + 4);
            d[6] = s[6];
            d[7] = (u8)(k + 1);
        }
}
/* 0x3A3C10 */
static void items_send(void)
{
    net_fields(6, QS8(0x22), 0);
    net_send();
}
/* 0x3A3C50: a state-2 check met when the master carries n of the item: quest_w+0x22 takes its value */
static void items_tick(void)
{
    u8 *d = quest_w + 0x1B0;
    int k;
    for (k = 0; k < 2; k++, d += 8) {
        if (*(s32 *)d != 2)
            continue;
        if ((s16)Pl_item_num_ck(PL(G_MASTER), *(u16 *)(d + 4)) < (s8)d[6])
            continue;
        if (QS8(0x22) < (s8)d[7]) {
            QS8(0x22) = (s8)d[7];
            items_send();
        }
        *(s32 *)d = 0;
    }
}

/* ------------------------------------------------ the timers */
/* 0x3A7EC0(n): the quest's time (not while game_w+0x275 is set or the start wait quest_w+0x156 runs), the counter +0x1C8
 * (up to 90000) and the countdown +0x544 */
static void timer_tick(int n)
{
    if (GU8(0x275) != 0 || QS16(0x156) > 0)
        return;
    if (Q_TIMER > 0) {
        Q_TIMER -= n;
        if (Q_TIMER < 0)
            Q_TIMER = 0;
    }
    if (QU16(0x1C4) != 0) {
        QS32(0x1C8) += n;
        if (QS32(0x1C8) > 90000)
            QS32(0x1C8) = 90000;
    }
    if (QS32(0x544) > 0) {
        QS32(0x544) -= n;
        if (QS32(0x544) < 0)
            QS32(0x544) = 0;
    }
}
/* 0x3A0760: when the countdown game_w+0x208 asked for ends, count it (+0x548) and tell the others */
static void countdown_tick(void)
{
    if (GU8(0x208) != 1 || QS32(0x544) > 0)
        return;
    GU8(0x208) = 0;
    if (QS32(0x544) == 0) {
        QS8(0x548)++;
        net_fields(18, QS8(0x548), 0);
        net_send();
    }
}
/* quest_timer_send 0x3AB5C0 */
static void timer_send(void)
{
    if (GU8(0x275) != 0)
        return;
    net_fields(5, (u32)Q_TIMER >> 16, Q_TIMER);
    net_send();
}
/* 0x3AB770 */
static void time_used(void) { QS32(0x52C) = Quest_time_get(1) - Quest_time_get(0); }

/* 0x3A3A60: an intrusion, by chance, once the time left falls under the quest's threshold (table 0x44B750[quest_w+0x9D],
 * byte pairs {minutes x 2 ... , chance in 128} by quest_w+0x9C): what intrudes is not read (a message and a sound) */
static void intrude_tick(void)
{
    u8 *t = ps2_ptr(0x44B750 + 4 * (uint32_t)QS8(0x9D));
    int n;
    if (QS8(0x149) != 1 || !t)
        return;
    t += 2 * QS8(0x9C);
    if (!t[0] || Q_TIMER > t[0] * 1800)     /* t[0] * 15 * 15 * 8 */
        return;
    QS8(0x9C)++;
    n = ran_suu(0) & 0x7F;
    if (n >= t[1])
        return;
    QS8(0x149) = 0;
    msg(0x44556C);
    se_req(1, 119, 0);
    net_fields(14, 0, 0);
    net_send();
}
/* game 0x6ED360: the master on game_w+0x31's stage with quest_w+0x224 set: notice 14 */
static void stage_notice(void)
{
    if (*(u16 *)(PL(G_MASTER) + 0x680) == GU8(0x31) && QU16(0x224) > 0)
        notice(14);
}

/* 0x3A0820: the slot of a monster kind in the quest's list (main 0x1BB9C8, 16 bytes) when the quest has flag 0x20 */
static int kind_slot(int kind)
{
    const u8 *l = rt_addr(0x1BB9C8, 16);
    int i;
    if (Online_ck() == 1 || !(Q_FLAGS & 0x20) || !l)
        return 0xFF;
    for (i = 0; i < 16; i++) {
        if (!l[i])
            return 0xFF;
        if (l[i] == (u8)kind)
            return i;
    }
    return 0xFF;
}
/* 0x39F540(side): an arena match ends: side 1 or 2 won */
static void match_end(int side)
{
    Q_SUB = 0;
    QS32(0x130) = 1800;
    Q_STEP = 3;
    QS32(0x12C) |= 1 << G_MASTER;
    if (same_side(PL(G_MASTER)[0x569], side, 1) == 1) {   /* 0x5F8DD9 = player_work + 0x569 */
        sysw33 = 0x81;
        Q_END = 4;
    } else
        Q_END = 6;
    net_fields(17, (s8)side, 0);
    QU16(0x558) = 0;
    net_send();
}
static int match_won(int n) { return (TF(n) & 0x31) == 0x31; }  /* 0x3A8990 */

/* ------------------------------------------------ the script, 0x3A9230 */
#define FIND(start, key)                                  \
    do {                                                  \
        QCMD *q_ = (start);                               \
        int k_ = (key);                                   \
        i = 0;                                            \
        for (;;) {                                        \
            if (q_->op == 0x1A && k_ == q_->a) {          \
                i++;                                      \
                break;                                    \
            }                                             \
            i++;                                          \
            q_++;                                         \
        }                                                 \
    } while (0)
#define SKIPTO(o)                \
    do {                         \
        i++;                     \
        p++;                     \
        for (;;) {               \
            if (p->op == (o)) {  \
                i++;             \
                break;           \
            }                    \
            i++;                 \
            p++;                 \
        }                        \
    } while (0)
static void hurt(void)
{
    u8 *w = Q_BOSS;
    if (w && (kind_slot(w[2]) & 0xFF) != 0xFF) {
        QS16(0x134) = *(s16 *)(w + 0x2C2);
        QU16(0x48C + 2 * w[2]) = *(u16 *)(w + 0x9A4);
    }
}
static s16 script(u8 *pl, s16 i)
{
    char buf[32];
    QCMD *p, *tbl;
    int r = -1, k;
    static s16 last = -2;

    if (G_STATE == 2 && Q_TIMER >= 0) {
        int n = QS8(0x97);
        const s32 *warn = n > 0 ? (const s32 *)rt_addr(0x44BDD0 + 8 * (uint32_t)n, 8) : NULL;
        if (warn && warn[0] >= Q_TIMER) {     /* the "N minutes left" messages (0x44BDD0, {time, ?}) */
            void *f = ps2_ptr(0x445558);
            if (f) {
                snprintf(buf, sizeof buf, (const char *)f, warn[0] / 1800);
                buf[31] = 0;
                set01_set2_use_mem(buf);
            }
            QS8(0x97)--;
        }
        timer_tick(1);
        if (Q_TIMER <= 0) {
            Q_TIMER = 0;
            i = 0;
            timer_send();
            Q_TIMER = -1;
            FIND(Q_CMD, QS8(0x0A));         /* time up: the label quest_w+0x0A */
        } else if (Q_TIMER % 9000 == 0)
            timer_send();
    }
    for (;;) {
        if (QS8(0x0B) != 0 && tgt_jump() == 1) {
            FIND(Q_CMD, QS8(0x0B));
            QS8(0x0B) = 0;
        }
        if (QS8(0x137) != 0 && QS8(0x138) != 0) {
            FIND(Q_CMD, (s16)QS8(0x137));
            QS8(0x137) = 0;
        }
        tbl = Q_CMD;
        p = &tbl[i];
        if (trace && i != last) {
            fprintf(stderr, "rt_mh2_quest: script %d: op %d (%d %d %d)\n", i, p->op, p->a, p->b, p->c);
            last = i;
        }
        switch (p->op) {
        case 1:
            Q_TGT_MODE = (u8)p->a;
            i++;
            continue;
        case 2:
            tgt_add(p->a, p->b, p->c);
            i++;
            continue;
        case 3:
            break;
        case 9:
            QS32(0x0C) = p->a;
            i++;
            break;
        case 10:
            if (--QS32(0x0C) <= 0)
                i++;
            break;
        case 11:
            set01_set2(mission_area + Q_MSGS[p->a]);
            i++;
            break;
        case 5:
            i++;
            continue;
        case 6:
            if (tgt_met(0) == 1 || tgt_sub_met() == 1) {
                i++;
                continue;
            }
            SKIPTO(7);
            continue;
        case 0x24:
            if ((s16)tgt_count((u16)p->a) > p->b)
                break;
            i++;
            continue;
        case 12:
            if (*(u16 *)(pl + 0x680) != (u16)p->a)
                break;
            i++;
            continue;
        case 4:
            Q_TIMER += (u16)p->a;
            i++;
            break;
        case -1:                        /* cleared */
            hurt();
            net_fields(2, 0, 0);
            sysw33 = 0x81;
            Q_END = 3;
            G_STATE = 3;
            i++;
            QS32(0x12C) |= 1 << (G_MASTER + 8);
            net_send();
            continue;
        case 0x2D:
            if ((QS32(0x12C) & 0xF00) != 0xF00 && G_PLSTATE(G_MASTER) == 1)
                break;
            i++;
            break;
        case 0x1E:
            Q_SUB = 0;
            i = -1;
            Q_STEP++;
            break;
        case 0x17:
            Q_PC = i = QS16(0x20);
            continue;
        case 0x18:
            if ((s16)act_ck(pl, 4, 0) == 0)
                break;
            i++;
            continue;
        case -2:                        /* failed */
            net_fields(4, 0, 0);
            G_STATE = 5;
            i++;
            Q_END = 5;
            net_send();
            continue;
        case 0x1F:                      /* time up */
            Q_END = 6;
            i = -1;
            G_STATE = 6;
            time_used();
            if (online_quest() == 0)
                msg(0x445560);
            if (online_quest() == 0 && qflag_200000() == 0 && qflag_4000000() == 0)
                info_set(0, 15, 0);
            break;
        case 0x1B: {
            void *f = ps2_ptr(0x445564);
            QS8(0x0A) = (s8)p->a;
            k = Quest_time_get(1);
            if (f) {
                snprintf(buf, sizeof buf, (const char *)f, k / 1800);
                buf[31] = 0;
                set01_set2_use_mem(buf);
            }
            if (online_quest() == 0)    /* set07_set 0x386CE0: MH2's makes a set object of type 18 run by sub_main 0x386DA0 (the
                                         * quest's start banner, guess); G's set07 is another object (it crashed here): not ported */
                rt_log_standin("MH2 set07_set (quest start object, type 18) not ported");
            i++;
            break;
        }
        case 0x20:
            QS8(0x22) = (s8)(u16)p->a;
            items_send();
            i++;
            continue;
        case 0x21:                      /* quest_item_ck3 0x3AAD20: the master carries b of item a */
            if ((s16)Pl_item_num_ck(PL(G_MASTER), (u16)p->a) < p->b)
                break;
            i++;
            continue;
        case 0x23:
            QS8(0x0B) = (s8)p->a;
            i++;
            break;
        case 0x2C:
            QS8(0x137) = (s8)p->a;
            i++;
            break;
        case 0x25:                      /* failed (other) */
            G_STATE = 5;
            i++;
            Q_END = 5;
            msg(0x445568);
            net_fields(10, 0, 0);
            net_send();
            continue;
        case 0x26:                      /* cleared (other) */
            hurt();
            net_fields(11, 0, 0);
            Q_END = 3;
            i++;
            G_STATE = 3;
            QS32(0x12C) |= 1 << (G_MASTER + 8);
            net_send();
            continue;
        case 0x28: {
            s16 h = Q_BOSS ? *(s16 *)(Q_BOSS + 0x2C2) : 0;
            if (h <= QS16(0x134) - p->a) {
                QS16(0x134) = h;
                i++;
                continue;
            }
            SKIPTO(0x29);
            continue;
        }
        case 0x2A:
            if ((Bdora_hp_ck() & 0xFF) == 1) {
                tgt_set(0, 0x200);
                i++;
                continue;
            }
            SKIPTO(0x2B);
            continue;
        case 0x1C:
            k = p->a;
            FIND(tbl, k);
            continue;
        case 0x27:
            break;
        case 0x1A:
            i++;
            continue;
        case 0x2F:
            QS8(0x0A) = (s8)p->a;
            i++;
            break;
        case 0x30:
            if ((s16)tgt_count((u16)p->a) <= p->b) {
                FIND(Q_CMD, p->c);
                continue;
            }
            i++;
            continue;
        case 0x31:
            r = i;
            i++;
            continue;
        case 0x32:
            r = -1;
            i++;
            continue;
        case 0x33: {
            const u8 *arena = rt_addr(0x6ADB2E, 1);     /* a byte before edit_w, not named (guess: 0 offline) */
            if (!arena || *arena == 0) {
                if (match_won(0) == 1) {
                    match_end(2);
                    i = -1;
                } else if (match_won(1) == 1) {
                    match_end(1);
                    i = -1;
                }
            } else if (QS8(0x152) <= 0) {
                match_end(2);
                i = -1;
            } else if (QS8(0x153) <= 0) {
                match_end(1);
                i = -1;
            }
            break;
        }
        case 0x34:
            tgt_set(0, 0);
            i++;
            continue;
        default:
            if (trace)
                fprintf(stderr, "rt_mh2_quest: script %d: op %d not known: waits\n", i, p->op);
            break;
        }
        break;
    }
    if (sysw33 == 0x80) {
        sysw33 = 0;
        if (tgt_done(0) == 1 && clear_ck(0) != 0) {
            /* sub_main 0x323C90 (0x3239B0(0)): not read */
        }
    }
    if ((s16)r != -1)
        i = (s16)r;
    return i;
}

/* ------------------------------------------------ the state machine, 0x3A2E30 (G's Quest_condition_judging) */
static void dump_script(void)
{
    QCMD *c = Q_CMD;
    int i;
    if (!c)
        return;
    fprintf(stderr, "rt_mh2_quest: quest %d: target mode %d, targets", Q_NO, Q_TGT_MODE);
    for (i = 0; i < 3; i++)
        fprintf(stderr, " {0x%X %d %d}", (unsigned)TF(i), TID(i), TCNT(i));
    fprintf(stderr, "\nrt_mh2_quest: script:");
    for (i = 0; i < 64; i++) {
        fprintf(stderr, " %d/%d/%d/%d", c[i].op, c[i].a, c[i].b, c[i].c);
        if (c[i].op == 0x1F || c[i].op == 0)
            break;
    }
    fprintf(stderr, "\n");
}
int Quest_condition_judging(void)
{
    static int said;
    u8 *me = PL(G_MASTER);
    int i, t;
    if (!said++) {
        trace = getenv("RT_QUEST_TRACE") != NULL;
        rt_log_standin("MH2 quest flow: 0x3A2E30 / 0x3A9230 as host C (rt_mh2_quest.c)");
        if (trace)
            dump_script();
    }
    for (i = 0; i < 4; i++)
        if (G_PLSTATE(i) != 1)
            QS32(0x12C) |= 1 << (i + 8);
    if (QS16(0x156) > 0)
        QS16(0x156)--;
    /* 0x3A5880 (MH2's timed pick points) skipped: see the top */
    if (Q_NO == 0)
        return 0;
    if (Q_END == 7 || Q_END == 8)
        return -2;
    switch (Q_STEP) {
    case 0:
        if (Q_END == 4)
            return 1;
        if (Q_END == 6)
            return -1;
        if (Q_PC == -1)
            return 1;
        if (online_quest() == 1)
            for (i = 0; i < G_PLNUM; i++)
                if (G_PLSTATE(i) != 1 && PL(i)[0x409] < 3)
                    error_set();
        {   /* test aid: RT_MH2_TARGET=N@T marks target N done on the T-th call (the clear path without a hunt) */
            static int calls;
            const char *e = getenv("RT_MH2_TARGET");
            calls++;
            if (e && strchr(e, '@') && calls == atoi(strchr(e, '@') + 1))
                tgt_set(atoi(e), 0);
        }
        tgt_tick();
        tgt_met(1);
        countdown_tick();
        Q_PC = script(me, Q_PC);
        intrude_tick();
        items_tick();
        if (qflag_c000000() == 1)
            stage_notice();
        return 0;
    case 1:
        switch (Q_SUB) {
        case 0:
            Q_SUB++;
            QS32(0x52C) = Quest_time_get(1) - Quest_time_get(0);
            net_fields(15, (u32)QS32(0x52C) >> 16, QS32(0x52C));
            net_send();
            Q_TIMER = 150;
            break;
        case 1:
            timer_tick(1);
            if (Q_TIMER <= 0) {
                Q_SUB++;
                if (tgt_done(0) == 1) {
                    if (Q_FLAGS & 0x20000006) {
                        Q_TIMER = 600;
                        msg(Online_ck() == 1 ? 0x445540 : 0x445544);
                    } else {
                        Q_TIMER = 1800;
                        msg(Online_ck() == 1 ? 0x445548 : 0x44554C);
                    }
                } else {
                    Q_TIMER = 300;
                    msg(Online_ck() == 1 ? 0x445598 : 0x44559C);
                }
            }
            break;
        case 2:
            timer_tick(1);
            if (Q_TIMER <= 0) {
                Q_SUB = 0;
                Q_STEP++;
                msg(0x445550);
                if (online_quest() == 0 && qflag_200000() == 0 && qflag_4000000() == 0)
                    info_set(0, 15, 0);
            }
            break;
        }
        break;
    case 2:
        switch (Q_SUB) {
        case 0: {
            u8 *rk;
            Q_SUB++;
            Q_END = 4;
            QS32(0x14) = QS32(0x14) / (G_PLNUM ? G_PLNUM : 1);
            rk = quest_w + 0xA0 + 2 * G_MASTER;
            *rk = Get_hunter_rank(User_data, QS32(0x14));
            net_fields(12, *rk, 0);
            net_send();
            QS32(0x130) = 5400;
            QS32(0x12C) |= 1 << G_MASTER;
            break;
        }
        case 1:
            if (QS32(0x130) > 0)
                QS32(0x130)--;
            for (i = 0; i < 4; i++)
                if (G_PLSTATE(i) != 1)
                    QS32(0x12C) |= 1 << i;
            if ((QS32(0x12C) & 0xF) == 0xF || QS32(0x130) <= 0 || G_PLSTATE(G_MASTER) != 1) {
                net_fields(13, (u32)QS32(0x128) >> 16, QS32(0x128));
                Q_SUB++;
                net_send();
                QS32(0x130) = 1800;
                QS32(0x12C) |= 1 << (G_MASTER + 4);
            }
            break;
        case 2:
            if (QS32(0x130) > 0)
                QS32(0x130)--;
            for (i = 0; i < 4; i++)
                if (G_PLSTATE(i) != 1)
                    QS32(0x12C) |= 1 << (i + 4);
            if ((u8)QS32(0x12C) == 0xFF || QS32(0x130) <= 0 || G_PLSTATE(G_MASTER) != 1) {
                G_STATE = 4;
                Q_SUB++;
            }
            break;
        case 3:
            break;
        }
        break;
    case 3:
        if (Q_SUB != 0)
            break;
        if (QS32(0x130) > 0)
            QS32(0x130)--;
        for (i = 0; i < 4; i++)
            if (G_PLSTATE(i) != 1)
                QS32(0x12C) |= 1 << i;
        if ((QS32(0x12C) & 0xF) == 0xF || QS32(0x130) <= 0 || G_PLSTATE(G_MASTER) != 1) {
            Q_SUB++;
            t = Quest_time_get(1);
            QS32(0x52C) = t - Quest_time_get(0);
            G_STATE = Q_END == 4 ? 4 : 6;
            for (i = 0; i < G_PLNUM; i++)
                if (PL(i)[0x409] < 3 && same_side(PL(i)[0x409], me[0x409], 1) == (Q_END == 4))
                    break;
            info_set(3, 3, (s16)i);
        }
        break;
    }
    if (trace) {
        static u32 old = 0xFFFFFFFF;
        u32 now = (u8)Q_STEP | (u8)Q_SUB << 8 | (u8)Q_END << 16 | (u32)G_STATE << 24;
        if (now != old) {
            fprintf(stderr, "rt_mh2_quest: step %d sub %d end %d state %d pc %d time %d\n", Q_STEP, Q_SUB, Q_END, G_STATE, Q_PC,
                    Q_TIMER);
            old = now;
        }
    }
    return 0;
}

/* Quest_start's parts that were skipped (rt_mh2.c calls this): 0x3A7FC0 and 0x3A3DF0 (clear), 0x3A8060 and 0x3A3E20 (fill) */
void rt_mh2_quest_targets_init(void)
{
    tgt_clear();
    items_clear();
    tgt_init();
    items_init();
}
#endif
