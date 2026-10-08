/* rt_np.c - direct-connect co-op glue (ONLINE=1 builds only; docs/network.md 1a and 3.4).
 *
 * The game's own in-quest code sends and receives (src/main/net/netsyn01.c net_send_pl,
 * netsyn02_nm.c net_receive_pl, netsyn03.c the network-delay slots). On the PS2 they sit on
 * the AQ / Cng / mcsls stack and Capcom's relaying game server; here the cut is at
 * AQ_data_put (send) and self_data_ctrl (receive), and src/pc/net/net_peer.c carries the
 * packets between the players directly, the host relaying.
 *
 * Slots are the game's own: the host is slot 0, joiners 1-3 in joining order;
 * game_w.master = this machine's slot (the PS2 sets it from CngNetAQConnectIdGet),
 * game_w.pl_num = the player count, game_w.pl_state[slot] = 1.
 *
 * Online_ck() is this file's `rt_online`: 0 until a co-op quest starts, so a single-player
 * run of mhview_online behaves as mhview.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif
#include "rt.h"
#include "types.h"
#include "game.h"
#include "pl.h"
#include "../net/net_peer.h"

#define PF(pl, T, o) (*(T *)((u8 *)(pl) + (o)))
#define GW8(o) (((u8 *)&game_w)[o])

extern u8 Ken_data[][0x18];
extern u8 Battle_type[];

static int rt_online;
static int want_hunter = -1;       /* --hunter N: the save slot (0-based), -1 = the first used */
static int want_role;               /* 1 host, 2 join (from the command line) */
static const char *want_addr;
static int want_port = NP_DEFAULT_PORT, want_players = 2;
static unsigned long st_tx, st_rx, st_drop;
static int trace = -1;

/* Online_ck (0x162D60): system_w+0x10 != 0 on the PS2 */
int Online_ck(void) { return rt_online; }
int rt_np_active(void) { return rt_online; }

static double now_s(void)
{
#ifdef _WIN32
    return GetTickCount() / 1000.0;
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
#endif
}

static void sleep_ms(int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    struct timespec t = { ms / 1000, (ms % 1000) * 1000000L };
    nanosleep(&t, NULL);
#endif
}

/* command line: --host [BIND_IP] / --join HOST_IP, --port N, --players N (host: wait for N) */
int rt_np_arg(int argc, char **argv, int *i)
{
    const char *a = argv[*i];
    if (!strcmp(a, "--host")) {
        want_role = 1;
        if (*i + 1 < argc && argv[*i + 1][0] >= '0' && argv[*i + 1][0] <= '9' && strchr(argv[*i + 1], '.'))
            want_addr = argv[++*i];
        return 1;
    }
    if (!strcmp(a, "--coop")) {         /* ask in dialogs */
        want_role = 3;
        return 1;
    }
    if (!strcmp(a, "--join") && *i + 1 < argc) {
        want_role = 2;
        want_addr = argv[++*i];
        return 1;
    }
    if (!strcmp(a, "--hunter") && *i + 1 < argc) {     /* the save slot (1-3) of this player's hunter */
        want_hunter = atoi(argv[++*i]) - 1;
        return 1;
    }
    if (!strcmp(a, "--port") && *i + 1 < argc) {
        want_port = atoi(argv[++*i]);
        return 1;
    }
    if (!strcmp(a, "--players") && *i + 1 < argc) {
        want_players = atoi(argv[++*i]);
        if (want_players < 1) want_players = 1;
        if (want_players > NP_MAX) want_players = NP_MAX;
        return 1;
    }
    return 0;
}

int rt_np_wanted(void) { return want_role; }



/* The Elder's offline quests, as the port has played them (docs/pc.md "All offline quests
 * played"; goals as the port's tests read them from the mission files, monster kinds by
 * their em number). Shown when --host is given without --quest. */
static const struct { int no, stars; const char *goal; } quests[] = {
    { 131, 1, "deliver items (the first gathering quest)" }, { 132, 1, "deliver items" }, { 133, 1, "deliver items" },
    { 134, 1, "deliver items" }, { 135, 1, "deliver items" },
    { 136, 2, "hunt 3 Velociprey (urgent)" }, { 141, 2, "deliver items" }, { 142, 2, "deliver items" }, { 143, 2, "deliver items" },
    { 138, 2, "deliver an egg" },
    { 137, 3, "Velocidrome (urgent)" }, { 152, 3, "Velocidrome" }, { 144, 3, "monster kind 6" }, { 148, 3, "monster kind 6" },
    { 145, 3, "10 Velociprey" }, { 146, 3, "deliver an egg" }, { 147, 3, "deliver items" }, { 149, 3, "deliver items" },
    { 150, 4, "item + monster kind 6" }, { 151, 4, "15 Velociprey + Rathalos" }, { 153, 4, "2 eggs" }, { 154, 4, "monster kind 8" },
    { 155, 4, "15 monsters kind 13" }, { 156, 4, "Gendrome" }, { 157, 4, "3 eggs" }, { 158, 4, "15 Vespoid" },
    { 159, 4, "deliver items" }, { 160, 4, "Iodrome" },
    { 139, 5, "Rathalos" }, { 140, 5, "item + Rathalos" }, { 161, 5, "20 Velociprey + Rathalos" }, { 162, 5, "Velocidrome" },
    { 163, 5, "3 eggs" }, { 165, 5, "20 monsters kind 13 + Plesioth" }, { 166, 5, "Gendrome" }, { 167, 5, "Iodrome" },
    { 168, 5, "Vespoid + Plesioth" }, { 171, 5, "Monoblos" },
};

static int quest_list_ask(void)
{
    char line[64];
    int k, q;
    fprintf(stderr, "\nCo-op host: pick the quest everyone plays (quest number, Enter = 131):\n");
    for (k = 0; k < (int)(sizeof quests / sizeof quests[0]); k++)
        fprintf(stderr, "  %3d  %d*  %s\n", quests[k].no, quests[k].stars, quests[k].goal);
    fprintf(stderr, "quest> ");
    if (!fgets(line, sizeof line, stdin))
        return 0;
    q = line[0] == '\n' ? 131 : (int)strtol(line, NULL, 0);
    if (q < 1 || q > 0xB1) {
        fprintf(stderr, "co-op: no quest %s", line);
        return 0;
    }
    return q;
}

/* --coop: ask host / join, the quest, the players or the host's address in small OS dialogs
 * (zenity or kdialog on Linux; the fixed command lines below never contain what the player
 * typed). Without either tool, or on Windows, the console asks instead. 0 = cancelled. */
#ifndef _WIN32
static int run_dialog(const char *cmd, char *out, int max)
{
    FILE *f = popen(cmd, "r");
    int n = 0;
    if (!f)
        return -1;
    if (fgets(out, max, f))
        n = (int)strlen(out);
    while (n > 0 && (out[n - 1] == '\n' || out[n - 1] == '\r'))
        out[--n] = 0;
    return pclose(f) == 0 ? n : -1;
}
static int have_tool(const char *t)
{
    char cmd[64];
    snprintf(cmd, sizeof cmd, "command -v %s >/dev/null 2>&1", t);
    return system(cmd) == 0;
}
#endif
static char ask_addr[64];
static int ask_coop(int *quest_no)
{
    char line[64];
#ifndef _WIN32
    int z = have_tool("zenity"), kd = !z && have_tool("kdialog");
    if (getenv("RT_NO_GUI"))
        z = kd = 0;
    if (z || kd) {
        char cmd[8192];
        int k, n;
        if (run_dialog(z ? "zenity --list --title='MH1 co-op' --text='Play a quest together' --column=Choice 'Host a quest' 'Join a host' 2>/dev/null"
                         : "kdialog --title 'MH1 co-op' --menu 'Play a quest together' host 'Host a quest' join 'Join a host' 2>/dev/null",
                       line, sizeof line) <= 0)
            return 0;
        if (line[0] == 'H' || line[0] == 'h') {
            n = snprintf(cmd, sizeof cmd, z ? "zenity --list --title='MH1 co-op: quest' --height=600 --column=Quest --column=Stars --column=Goal"
                                            : "kdialog --title 'MH1 co-op: quest' --menu 'The quest everyone plays'");
            for (k = 0; k < (int)(sizeof quests / sizeof quests[0]); k++)
                n += snprintf(cmd + n, sizeof cmd - n, z ? " %d %d '%s'" : " %d '%d* %s'", quests[k].no, quests[k].stars, quests[k].goal);
            snprintf(cmd + n, sizeof cmd - n, " 2>/dev/null");
            if (run_dialog(cmd, line, sizeof line) <= 0)
                return 0;
            *quest_no = atoi(line);
            if (run_dialog(z ? "zenity --scale --title='MH1 co-op' --text='Players (you included)' --min-value=2 --max-value=4 --value=2 2>/dev/null"
                             : "kdialog --title 'MH1 co-op' --menu 'Players (you included)' 2 2 3 3 4 4 2>/dev/null", line, sizeof line) > 0)
                want_players = atoi(line) < 2 ? 2 : atoi(line) > 4 ? 4 : atoi(line);
            return 1;
        }
        if (run_dialog(z ? "zenity --entry --title='MH1 co-op' --text=\"The host computer's LAN address (e.g. 192.168.1.20)\" --entry-text=127.0.0.1 2>/dev/null"
                         : "kdialog --title 'MH1 co-op' --inputbox 'The host computer LAN address (e.g. 192.168.1.20)' 127.0.0.1 2>/dev/null",
                       ask_addr, sizeof ask_addr) <= 0)
            return 0;
        want_addr = ask_addr;
        return 2;
    }
#endif
    fprintf(stderr, "Co-op: (h)ost a quest or (j)oin a host? ");
    if (!fgets(line, sizeof line, stdin))
        return 0;
    if (line[0] == 'h' || line[0] == 'H') {
        *quest_no = quest_list_ask();
        fprintf(stderr, "players (2-4, Enter = 2): ");
        if (fgets(line, sizeof line, stdin) && atoi(line) >= 2 && atoi(line) <= 4)
            want_players = atoi(line);
        return *quest_no ? 1 : 0;
    }
    fprintf(stderr, "host address (Enter = 127.0.0.1): ");
    if (!fgets(ask_addr, sizeof ask_addr, stdin))
        return 0;
    ask_addr[strcspn(ask_addr, "\r\n")] = 0;
    if (!ask_addr[0])
        strcpy(ask_addr, "127.0.0.1");
    want_addr = ask_addr;
    return 2;
}

/* ------------------------------------------------ the player's own save
 * A co-op player uses the hunter of his own memory card (the PC's card directory, rt_mc.c):
 * the save image BISLPM-65495MH is decoded as decode_data does (mccomb.c; header u16 0x100,
 * key, checksum, 0x5963, then 0x8A20 words XORed with key = key * 0xB0 % 65363), the options
 * and the three hunter slots go into option_w as a Continue does (save_data_sub(0, ...)),
 * and the slot's data into User_data (Load_userdata); select_w[0xB6] = the slot, as the
 * Continue screen sets it, so the village's bed save writes the right slot. After the quest
 * the slot is written back to the card (rt_np_session_end). --hunter N picks the slot (1-3),
 * else the first used one. Without a save the hunter comes from RT_WEAPON / RT_PL_LOOK. */
#define SAVE_SIZE 0x11450
extern u8 *data_load_ptr;
extern u8 option_w[];
extern u8 select_w[];
extern u8 User_data[];
int save_data_sub(int save, int mask);
void Load_userdata(int slot);
void Save_userdata(int slot);
void encode_data_002814E0(void *buf);
const char *rt_mc_root(void);
static int save_slot = -1;          /* the card slot this player's hunter came from, -1 none */

static void save_path(char *out, size_t n)
{
    snprintf(out, n, "%s/BISLPM-65495MH/BISLPM-65495MH", rt_mc_root());
}

static int save_decode(u8 *b)
{
    u16 *w = (u16 *)b, key, stored, sum = 0;
    int i;
    if (w[0] != 0x100)
        return -1;
    key = w[1];
    stored = w[2];
    for (i = 0; i < 0x8A20; i++) {
        w[4 + i] ^= key;
        sum = (u16)(sum + w[4 + i]);
        if (key == 0)
            key = 1;
        key = (u16)((key * 0xB0) % 65363);
    }
    return sum == stored ? 0 : -2;
}

static u8 *save_read(void)
{
    char path[600];
    FILE *f;
    u8 *b;
    save_path(path, sizeof path);
    if (!(f = fopen(path, "rb")))
        return NULL;
    b = calloc(1, 0x12000);
    if (fread(b, 1, SAVE_SIZE, f) != SAVE_SIZE || save_decode(b) != 0) {
        fprintf(stderr, "co-op: %s is not a readable save\n", path);
        free(b);
        b = NULL;
    }
    fclose(f);
    return b;
}

static int load_own_hunter(void)
{
    u8 *b = save_read(), *keep = data_load_ptr;
    int s;
    if (!b)
        return -1;
    data_load_ptr = b;
    save_data_sub(0, 0xF);          /* options + the three slots into option_w (not the patch) */
    data_load_ptr = keep;
    free(b);
    for (s = 0; s < 3; s++)
        if ((want_hunter < 0 || want_hunter == s) && option_w[0x10 + s * 0x480] != 0)
            break;
    if (s == 3) {
        fprintf(stderr, "co-op: no hunter in %s slot %d\n", want_hunter < 0 ? "any" : "the chosen", want_hunter + 1);
        return -1;
    }
    Load_userdata(s);
    select_w[0xB6] = (u8)s;
    save_slot = s;
    fprintf(stderr, "co-op: hunter \"%.18s\" from save slot %d (%d zenny)\n", (char *)User_data + 8, s + 1, *(s32 *)(User_data + 0x20));
    return 0;
}

/* the player's hunter back into his own save (the bed save's steps: Save_userdata,
 * save_data_sub(1, ...), encode_data; only this slot changes) */
static void save_own_hunter(void)
{
    char path[600];
    u8 *b, *keep = data_load_ptr;
    FILE *f;
    if (save_slot < 0 || !(b = save_read()))
        return;
    Save_userdata(save_slot);
    data_load_ptr = b;
    save_data_sub(1, 2 << save_slot);
    data_load_ptr = keep;
    encode_data_002814E0(b);
    save_path(path, sizeof path);
    if ((f = fopen(path, "wb")) != NULL) {
        fwrite(b, 1, SAVE_SIZE, f);
        fclose(f);
        fprintf(stderr, "co-op: hunter saved to slot %d (%d zenny)\n", save_slot + 1, *(s32 *)(User_data + 0x20));
    }
    free(b);
}

/* the mini data of the saved hunter (User_data, as Set_userdata / Set_equip_data read it) */
u8 Get_weapon_id(void *e);
#define Get_weapon_id_u(p) Get_weapon_id(p)
static void mini_from_save(u8 *m)
{
    const u8 *u = User_data;
    int wid;
    memset(m, 0, NP_MINI);
    m[3] = u[1];
    *(s32 *)(m + 4) = *(const s32 *)(u + 4);
    memcpy(m + 8, u + 0x3CC, 6);        /* the weapon triple */
    wid = *(const u16 *)(u + 0x3CE);
    m[0] = Battle_type[Get_weapon_id_u(m + 8)];
    m[0xE] = u[0x3D2];
    m[0xF] = (u8)(u[2] + 1);
    memcpy(m + 0x10, u + 0x3D3, 4);
    m[0x14] = u[3];
    m[0x16] = u[0x3D7];
    memcpy(m + 0x18, u + 8, 0x12);      /* the name */
    (void)wid;
}

/* This player's mini data (Lb_set_mini_data's layout, lb_village_nm.c; docs/network.md 1a):
 *   0 weapon job, 3 sex (PLW+0x11), 4 s32 hair colour (+0x5FC), 8/A/C the weapon triple
 *   (+0x35E/0x360/0x362: type 6 sword or 7 gun in the high byte of the first, the id),
 *   E..13 armour and face (+0x352..0x357: legs, face + 1, head, body, arms, waist),
 *   0x14 hair style (+0x34E), 0x16 (+0x8D3).
 * The PS2 builds it from the hunter in the town; the co-op start has no town, so it comes
 * from RT_WEAPON and RT_PL_LOOK="sex,face,hair,legs,head,body,arms,waist[,hair colour hex]"
 * (armour ids as Armor_*_Data rows, 0 = none; face and hair from 1; default: male, armour 5 all over:
 * there is no save in a co-op start yet). */
static void make_mini(u8 *m)
{
    int v[9] = { 0, 1, 1, 5, 5, 5, 5, 5, 0 }, wid = getenv("RT_WEAPON") ? atoi(getenv("RT_WEAPON")) : 156;
    const char *l = getenv("RT_PL_LOOK");
    if (l)
        sscanf(l, "%d,%d,%d,%d,%d,%d,%d,%d,%x", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], (unsigned *)&v[8]);
    if (wid <= 0 || wid >= 234)
        wid = 156;
    memset(m, 0, NP_MINI);
    m[0] = Battle_type[Ken_data[wid][0]];
    m[3] = (u8)(v[0] != 0);
    *(s32 *)(m + 4) = v[8];
    *(s16 *)(m + 8) = 6 << 8;           /* +0x35E = 0, +0x35F = 6 (sword) */
    *(u16 *)(m + 0xA) = (u16)wid;
    m[0xE] = (u8)v[3];
    m[0xF] = (u8)(v[1] > 0 ? v[1] : 1);
    m[0x10] = (u8)v[4];
    m[0x11] = (u8)v[5];
    m[0x12] = (u8)v[6];
    m[0x13] = (u8)v[7];
    m[0x14] = (u8)(v[2] > 0 ? v[2] - 1 : 0);
}

/* Set_mini_data_to_pl (f_ud.c, not in the PC build): a player's equipment and look from
 * the mini data, as init_pl_work does for the other players online */
static void apply_mini(PLW *pl, const u8 *m)
{
    u8 *p = (u8 *)pl;
    *(s32 *)(p + 0x5FC) = *(const s32 *)(m + 4);
    p[0x11] = m[3];
    p[0x34E] = m[0x14];
    *(s16 *)(p + 0x35E) = *(const s16 *)(m + 8);
    *(s16 *)(p + 0x360) = *(const s16 *)(m + 0xA);
    *(s16 *)(p + 0x362) = *(const s16 *)(m + 0xC);
    p[0x8D3] = m[0x16];
    p[0x34C] = Get_weapon_id(p + 0x35E);
    memcpy(p + 0x352, m + 0xE, 6);
    pl->kind = Battle_type[pl->work34C];
    if (m[0x18])
        memcpy(pl->name, m + 0x18, sizeof pl->name);
}

/* Before the quest is set up: host waits for the joiners and announces the quest; a joiner
 * connects and waits for it. Returns the quest number, 0 = no co-op, -1 = failed. */
int rt_np_setup(int quest_no)
{
    u8 mini[NP_MINI];
    int wait_s = getenv("RT_NP_WAIT") ? atoi(getenv("RT_NP_WAIT")) : 300, t;
    if (!want_role)
        return 0;
    if (want_role == 3) {
        want_role = ask_coop(&quest_no);
        if (!want_role)
            return -1;
    }
    if (getenv("RT_NP_NOSAVE") || load_own_hunter() != 0)
        make_mini(mini);            /* no save: RT_WEAPON / RT_PL_LOOK */
    else
        mini_from_save(mini);
    if (want_role == 1) {
        if (!quest_no)
            quest_no = quest_list_ask();    /* the list on the console */
        if (!quest_no) {
            fprintf(stderr, "co-op: --host needs a quest (--quest N, or pick one from the list)\n");
            return -1;
        }
        if (np_host(want_addr, want_port, mini) != 0)
            return -1;
        fprintf(stderr, "co-op: quest %d, waiting for %d more player(s) on port %d\n", quest_no, want_players - 1, want_port);
        for (t = 0; np_players() < want_players && t < wait_s * 100; t++) {
            np_poll();
            sleep_ms(10);
        }
        np_poll();
        if (np_players() < want_players)
            fprintf(stderr, "co-op: only %d player(s) after %d s, starting anyway\n", np_players(), wait_s);
        sleep_ms(100);          /* the joiners' HELLO (their mini data) */
        np_poll();
        np_host_start(quest_no);
    } else {
        if (np_join(want_addr, want_port, mini) != 0)
            return -1;
        for (t = 0; !np_started() && np_connected(0) && t < wait_s * 100; t++) {
            np_poll();
            sleep_ms(10);
        }
        if (!np_started()) {
            fprintf(stderr, "co-op: the host did not start a quest\n");
            return -1;
        }
        quest_no = np_quest();
    }
    fprintf(stderr, "co-op: quest %d, %d player(s), this is slot %d\n", quest_no, np_players(), np_slot());
    return quest_no;
}

int rt_np_slot(void) { return want_role ? np_slot() : 0; }
int rt_np_players(void) { return want_role ? np_players() : 1; }

/* rt_player_game_init, before pl_init: the session's slots, as Game_task step 3 and
 * init_pl_work do online (every slot's equipment and look from its mini data, which the host
 * collected and sent to all). */
void rt_np_init_slots(void)
{
    int n = np_players(), me = np_slot(), s;
    if (!want_role || !np_started())
        return;
    game_w.master = (u8)me;
    game_w.pl_num = (u8)n;
    {   /* the HUD's own hunter (Pit_init: lpPit->pl = player_work[game_w.master]; it ran before the
         * slots were known) */
        extern u8 *lpPit;
        if (lpPit)
            *(PLW **)(lpPit + 8) = &player_work[me];
    }
    GW8(0x21B) = 0;                 /* host slot (host_change; guess) */
    GW8(0x1B0) = 2;                 /* frames of network delay (net_start_ck computes it from the ping) */
    for (s = 0; s < 8; s++)
        game_w.pl_state[s] = s < n ? 1 : 0;
    {   /* the quest's first monsters were made (pull_enemy_work, em_init) before the slots were known:
         * give them this machine's slot as those do (+0x8C3: 0 = the host owns them, +0x88E) */
        extern u8 em_work[];
        int k;
        for (k = 0; k < 20; k++)
            if (em_work[0xA10 * k]) {
                em_work[0xA10 * k + 0x8C3] = (u8)me;
                em_work[0xA10 * k + 0x88E] = (u8)me;
            }
    }
    for (s = 0; s < n; s++) {
        PLW *pl = &player_work[s];
        if (s != me) {
            pl->be_flag = 1;
            pl->id = (u16)s;
            pl->stg = game_w.stage;
            PF(pl, u8, 0x10) = 0;
        }
        snprintf(pl->name, sizeof pl->name, "HUNTER %d", s + 1);   /* without a save */
        apply_mini(pl, np_mini(s));
        if (s == me && save_slot >= 0) {
            void Set_userdata(PLW *pl);
            Set_userdata(pl);       /* name, pouch, equipment from the save (init_pl_work's own-player path) */
        }
    }
    {   /* the same random numbers to start with on every machine (the PS2 seeds them from its
         * clock, init_ran_suu): the quest's set-up then places the same things */
        extern u16 Rnd_w[];
        Rnd_w[0] = Rnd_w[1] = (u16)(0x1234 + np_quest());
    }
    if (getenv("RT_QUEST_TIME")) {  /* test aid: the quest's time left (ticks), for time-out tests */
        extern u8 quest_w[];
        *(s32 *)(quest_w + 0x10) = atoi(getenv("RT_QUEST_TIME"));
    }
    rt_online = 1;
    if (trace < 0)
        trace = getenv("RT_NP_TRACE") != NULL;
}

/* after pl_init: every hunter's models from its equipment (player_all_load ->
 * armor_create_model; the viewer builds the look rt_player_look reports) */
void armor_create_model(void *pl);
void rt_np_after_init(void)
{
    int s;
    if (!rt_online)
        return;
    for (s = 0; s < game_w.pl_num; s++)
        armor_create_model(&player_work[s]);
    for (s = 0; s < game_w.pl_num; s++) {     /* who is who (names in hex: the game writes Shift-JIS) */
        const u8 *n = (const u8 *)player_work[s].name;
        int k;
        fprintf(stderr, "co-op: slot %d%s name", s, s == game_w.master ? " (me)" : "");
        for (k = 0; k < 0x12 && n[k]; k++)
            fprintf(stderr, " %02X", n[k]);
        fprintf(stderr, " weapon %d armour %d %d %d %d %d\n", PF(&player_work[s], u16, 0x360), PF(&player_work[s], u8, 0x352),
                PF(&player_work[s], u8, 0x354), PF(&player_work[s], u8, 0x355), PF(&player_work[s], u8, 0x356), PF(&player_work[s], u8, 0x357));
    }
    {   /* the start barrier (net_start_ck's job, game13): every player has loaded the stage
         * before anyone's hunt starts, so slow machines do not miss the first packets */
        static int round;
        int t;
        round++;
        for (t = 0; np_ready(round) < np_players() && t < 3000; t++) {
            np_poll();
            sleep_ms(10);
        }
        if (t >= 3000)
            fprintf(stderr, "co-op: not every player was ready after 30 s, starting anyway\n");
    }
}

/* mcsls_force_drop (mcsls, from net_receive_sys kind 12: a player abandoned the quest):
 * the session layer drops him, which the game sees as pl_state 0xFF (his monsters are then
 * handed on by Em_Master_Change) */
void mcsls_force_drop(u8 slot)
{
    if (slot < game_w.pl_num && slot != game_w.master && game_w.pl_state[slot] != 0xFF) {
        game_w.pl_state[slot] = 0xFF;
        player_work[slot].x01 = 0;
        fprintf(stderr, "co-op: player %d abandoned the quest\n", slot);
    }
}

/* AQ_data_put (aq_nm.c): queue a game packet for everyone. Byte 1 is the packet's length
 * (header included), byte 2 the sender's slot. */
int AQ_data_put(int ch, u8 *d, int mode)
{
    int n = d[1];
    (void)mode;
    if (!rt_online)
        return -1;
    if (ch <= 0 || ch >= 0xB)
        return -4;
    d[2] = game_w.master;
    if (n < 4)
        n = 4;
    if (n > NP_PKT_MAX)
        return -6;
    np_send(ch, d, n);
    if (ch == 7)            /* the host channel: the sender handles its own packets too (self_data_ctrl) */
        np_loopback(ch, d, n);
    st_tx++;
    if (trace)
        fprintf(stderr, "np: tx ch %d kind %d len %d\n", ch, d[0], n);
    return 0;
}

/* net_start_ck (netsyn07_nm.c, game13): the quest start was agreed before the stage loaded */
int net_start_ck(void) { return 1; }

void net_receive_pl(int no, u8 *buf, int force);
void net_receive_em(int slot, u8 *buf);
void net_receive_sys(int slot, u8 *buf);
void net_receive_host(int slot, u8 *buf);

/* netsyn11_nm.c calls game.bin functions by address (m2c names): the named ones */
void em_type_act_set(void *em, int kind, u16 no, u16 arg);
void em_ikari_add(void *em, s16 n);
void ikari_flag_set(void *em);
void target_kind_set(void *em, f32 *pos);
void em_area_move_init(void *em);
void em_hungry_add(void *em, s32 n);
void em_hagitori_lv_up(void *em, u8 bits);
void em_tail_off_sub(void *em);
void em_niku_eat_set(void *em);
void em_eye_dmg_act_set(void *em);
void poison_stock_set(void *em, int n);
void em_cmd_reset(void *em);
int Em_Mode_Chg(void *em, int mode, s16 mode2);
extern s16 em_atk_mode_timer_tbl[];
void func_535A10(void *em, int a, int b, int c) { em_type_act_set(em, a, (u16)b, (u16)c); }
void func_536110(void *em, s16 n) { em_ikari_add(em, n); }
void func_536320(void *em) { ikari_flag_set(em); }
void func_537620(void *em, void *pos) { target_kind_set(em, pos); }
void func_538580(void *em) { em_area_move_init(em); }
void func_53A630(void *em, int n) { em_hungry_add(em, n); }
void func_53B6D0(void *em, u8 bits) { em_hagitori_lv_up(em, bits); }
void func_53B8A0(void *em) { em_tail_off_sub(em); }
void func_53B9C0(void *em) { em_niku_eat_set(em); }
void func_559320(void *em) { em_eye_dmg_act_set(em); }
void func_55A440(void *em, s16 n) { poison_stock_set(em, n); }
void func_5655D0(void *em) { em_cmd_reset(em); }
void func_566500(void *em, int mode, int mode2) { Em_Mode_Chg(em, mode, (s16)mode2); }

/* every game tick, before the players move: the received packets, as AQ_recv /
 * self_data_ctrl dispatch them */
void rt_np_tick(void)
{
    static u8 buf[NP_PKT_MAX];
    int from, type, n;
    if (!want_role)
        return;
    if (rt_online) {
        /* System_timer: the PS2's Scheduler counts it every frame; the PC's rt_sys_tick
         * does not, and pl_move_sub sends the position packet (kind 2) when
         * System_timer % 20 == this slot */
        extern u16 System_timer;
        static double t0;
        static long n_ticks;
        double t;
        System_timer++;
        /* all players run at the PS2's 30 ticks a second, also headless runs (which
         * otherwise run as fast as they can and leave the others behind) */
        t = now_s();
        if (n_ticks == 0 || t - (t0 + n_ticks / 30.0) > 0.5)
            t0 = t - n_ticks / 30.0;            /* start, or far behind: no catching up */
        else if (t0 + n_ticks / 30.0 > t)
            sleep_ms((int)((t0 + n_ticks / 30.0 - t) * 1000.0));
        n_ticks++;
    }
    np_poll();
    if (rt_online && getenv("RT_QUEST_RETIRE")) {     /* test aid: abandon the quest at that tick (the quest menu's
                                                       * "return to the village": Quest_retire_set) */
        static int tk;
        void Quest_retire_set(void);
        if (++tk == atoi(getenv("RT_QUEST_RETIRE"))) {
            fprintf(stderr, "co-op: tick %d abandoning the quest\n", tk);
            Quest_retire_set();
        }
    }
    if (rt_online) {        /* a player who left: as mcsls_force_drop marks it (pl_state 0xFF), and no longer shown */
        int s;
        for (s = 0; s < game_w.pl_num; s++)
            if (s != game_w.master && np_gone(s) && game_w.pl_state[s] != 0xFF) {
                game_w.pl_state[s] = 0xFF;
                player_work[s].x01 = 0;
                fprintf(stderr, "co-op: player %d left the quest\n", s);
            }
    }
    while ((n = np_recv(&from, &type, buf, sizeof buf)) >= 0) {
        st_rx++;
        if (n < 4 || (buf[2] == game_w.master && type != 7)) {
            st_drop++;
            continue;
        }
        if (trace)
            fprintf(stderr, "np: rx from %d ch %d kind %d len %d\n", from, type, buf[0], n);
        if (!rt_online)
            continue;
        switch (type) {
        case 1: case 2: case 3: case 4:
            if (type - 1 != buf[2] || type - 1 >= game_w.pl_num) {
                st_drop++;
                break;
            }
            net_receive_pl(type, buf, 0);
            break;
        case 7:             /* the supply box, decided by the host (net_send_host) */
            net_receive_host(type, buf);
            break;
        case 8:             /* monsters: the owner's actions, HP, hand-over (net_send_em) */
            net_receive_em(type, buf);
            break;
        case 10:            /* session / quest events (net_send_sys: quest clear, fail, timer, kills ...) */
            net_receive_sys(type, buf);
            break;
        default:            /* 6 chat: not wired */
            st_drop++;
            break;
        }
    }
    {
        static int k;
        int s;
        ++k;
        if (trace && k % 300 == 0)
            fprintf(stderr, "np: %lu sent, %lu received, %lu not used\n", st_tx, st_rx, st_drop);
        if (getenv("RT_NP_EM") && rt_online && k % atoi(getenv("RT_NP_EM")) == 0) {   /* test aid: the monsters as this machine has them */
            extern u8 em_work[];
            for (s = 0; s < 20; s++) {
                u8 *e = em_work + 0xA10 * s;
                if (!e[0] || e[2] == 0)
                    continue;
                fprintf(stderr, "np-em: tick %d me %d em %d kind %d stg %d hp %d owner %d net %d act %d/%d pos %.0f %.0f\n", k, game_w.master, s,
                        e[2], e[0x736], *(s16 *)(e + 0x302), e[0x8C3] ? -1 : game_w.master, e[0x9E2], e[0x14], e[0x15],
                        *(f32 *)(e + 0xAC), *(f32 *)(e + 0xB4));
            }
        }
        if (getenv("RT_NP_POS") && rt_online && k % atoi(getenv("RT_NP_POS")) == 0)   /* test aid: every player's position as this machine sees it */
            for (s = 0; s < game_w.pl_num; s++) {
                PLW *p = &player_work[s];
                fprintf(stderr, "np-pos: tick %d me %d slot %d stg %d pos %.0f %.0f %.0f ang %04X/%04X act %d/%d box %08X pouch %d:%d\n", k,
                        game_w.master, s, p->stg, p->pos[0], p->pos[1], p->pos[2], p->ang[1] & 0xFFFF, p->ang_y & 0xFFFF, p->flag14,
                        p->flag15, *(u32 *)((u8 *)&game_w + 0x1A8), PF(p, s16, 0x828), PF(p, s16, 0x82A));
            }
    }
}

/* After the quest (game mode 6, back to the village): the player's hunter into his own
 * save, then single player again (the village is offline): Online_ck off, this machine's
 * hunter in slot 0, the connection closed. Returns the slot the hunter had, -1 = no session
 * (the viewer then draws slot 0 again). */
void ItemCopy_Pl2Ud(PLW *pl);
int rt_np_session_end(void)
{
    int me = game_w.master, s;
    if (!want_role || !rt_online)
        return -1;
    ItemCopy_Pl2Ud(&player_work[me]);       /* the pouch as it is now */
    save_own_hunter();
    rt_online = 0;
    np_close();
    /* (the village sets player_work[0] up again from User_data: Clear_lobby_ram, Local_main) */
    for (s = 1; s < 8; s++) {
        player_work[s].be_flag = 0;
        player_work[s].x01 = 0;
        game_w.pl_state[s] = 0;
    }
    player_work[0].id = 0;
    game_w.master = 0;
    game_w.pl_num = 1;
    game_w.pl_state[0] = 1;
    fprintf(stderr, "co-op: session over, back to single player\n");
    return me;
}

void rt_np_close(void)
{
    if (want_role)
        np_close();
}

/* the other player in `slot` is in the quest and on this player's area */
int rt_np_visible(int slot)
{
    PLW *p = &player_work[slot & 7], *me = &player_work[game_w.master];
    return rt_online && slot != game_w.master && slot < game_w.pl_num && p->be_flag && p->x01 && p->stg == me->stg;
}
