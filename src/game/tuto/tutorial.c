/* tutorial - game.bin 0x0063ACA0-0x0063B020: Tutorial_prog to soncho_no_oshie.
 * Tutorial quests: sets event flags when the player reaches a stage or
 * does a step, and has the village chief ("soncho") announce it. */
#include "types.h"
#include "game.h"
#include "pl.h"

/* quest_w (0x3C7440): only the quest number is used here. */
typedef struct QUEST_W {
    u8 _pad00[8];
    s16 no;             /* 0x08 current quest */
} QUEST_W;

/* quest_t_stin_tbl: per quest, rows of 8 bytes {stage, flag, ... -1}. */
typedef struct TUTO_STIN {
    s32 quest;          /* -1 ends the table */
    s8 *rows;
} TUTO_STIN;

extern QUEST_W quest_w;
extern PLW player_work[];
extern TUTO_STIN quest_t_stin_tbl[];
extern u8 quest_t001_tbl[];
extern char lit_173_0068A530[];

int Event_flag_ck(int);
void Event_flag_set(int);
void Event_flag_clear(int);
int Get_tuto_id(void *);
u8 set01_set2(char *str);
u16 ran_suu(int);
void se_req(int, int, int);

static int tuto_stage_in_flag_set(int quest, PLW *pl);
static int tuto_flag_set_sub(int flag);
static void soncho_no_oshie(void);

void Tutorial_prog(void) {
    PLW *pl = &player_work[game_w.master];
    int flag;
    int id;

    flag = tuto_stage_in_flag_set(quest_w.no, pl);
    switch (quest_w.no) {
    case 0x83:
        if (pl->stg == 0x15) {
            if ((id = Get_tuto_id(quest_t001_tbl)) != 0 && Event_flag_ck(id) == 0) {
                Event_flag_set(id);
                Event_flag_clear(id + 0x50);
                flag = 1;
            }
        }
        break;
    case 0x84:
    case 0x85:
        break;
    }
    if (flag != 0) {
        soncho_no_oshie();
    }
}

void Tuto_flag_set(int kind) {
    int flag = 0;

    switch (kind) {
    case 0:
        switch (quest_w.no) {
        case 0x83:
            flag = tuto_flag_set_sub(0x6A);
            break;
        case 0x84:
            tuto_flag_set_sub(0x6D);
            flag = tuto_flag_set_sub(0x6E);
            break;
        }
        break;
    }
    if (flag != 0) {
        soncho_no_oshie();
    }
}

static int tuto_stage_in_flag_set(int quest, PLW *pl) {
    TUTO_STIN *t;
    s8 *row;
    int i;
    int flag;

    t = quest_t_stin_tbl;
    flag = 0;
    for (; t->quest != -1; t++) {
        if (t->quest == quest) {
            for (row = t->rows; row[0] != -1; row += 8) {
                if (row[0] == pl->stg) {
                    for (i = 1; i < 8; i++) {
                        if (row[i] == -1) {
                            break;
                        }
                        flag = tuto_flag_set_sub(row[i] + 100);
                    }
                }
            }
            break;
        }
    }
    return flag;
}

static int tuto_flag_set_sub(int flag) {
    if (Event_flag_ck(flag) == 0) {
        Event_flag_set(flag);
        Event_flag_clear(flag + 0x50);
        return 1;
    }
    return 0;
}

int Tutorial_quest_ck(void) {
    if (quest_w.no >= 0x83 && quest_w.no < 0x8B) {
        return 1;
    }
    if (quest_w.no == 0x94 || quest_w.no == 0x96) {
        return 1;
    }
    return 0;
}

int Tutorial_flag_set(int flag) {
    if (tuto_flag_set_sub(flag)) {
        soncho_no_oshie();
        return 1;
    }
    return 0;
}

static void soncho_no_oshie(void) {
    set01_set2(lit_173_0068A530);
    se_req(7, (ran_suu(1) & 3) + 0x28, 0);
}
