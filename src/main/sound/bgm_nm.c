/* bgm_nm - BGM control (SLPM_654.95 0x0021D470-0x0021E260, main.bin) as near-match C, not built; matching runs are
 * built from it: bgm01 (bgm_server..die_bgm_set), bgm02 (lobby_bgm_set2..demo_bgm_set).
 * Not matching: em_status_ck (15, second loop register order), lobby_bgm_set and stage_bgm_set (the two flSndSetRev
 * argument loads come out in the other order; stage_bgm_set 4 diffs). Raw byte offsets into game_w. */
#include "types.h"

extern u8 game_w[];
extern u8 player_work[];
extern u8 em_work[];
extern u8 quest_w[];
extern u8 Snd_bgm_tbl[];
extern u8 Snd_rev_data_tbl[];
extern s32 Snd_rev_set_tbl[][4];
extern u8 fight_bgm_tbl[8];
extern u8 stage_bgm_etc_tbl[];

void fight_bgm_set();
void stage_bgm_set();
int Pl_master_ck();
int Quest_clear_bit_ck();
int Quest_clear_ck();
int flSndSetRev();
int str_fadein();
int str_fadein_vol();
int str_fadeout();
int str_getstat();
int str_pause();
int str_play();
int str_play_f_vol();
int str_stop();
int str_volume();

int em_status_ck(void) {
    u8 *p;
    s16 i;
    u8 *e = em_work;
    u8 *pp = player_work + game_w[0xD1] * 0xA00;
    s16 j;

    if (game_w[0x21F] != 0) {
        return 0;
    }
    if (*(s32 *)(quest_w + 0x40) & 0x600) {
        return 0;
    }
    for (i = 0; i < 20; i++) {
        if (e[0] != 0 && e[1] != 0 && e[0x612] >= 2 && e[0x14] != 5 && e[0x888] == 1
            && e[0x736] == pp[0x736] && e[2] != 0x1D) {
            return 2;
        }
        e += 0xA10;
    }
    p = player_work;
    for (j = 0; j < game_w[0xD3]; j++) {
        if (p[0] != 0 && p[0x81E] != 0 && p[0x736] == game_w[0x14]) {
            return 1;
        }
        p += 0xA00;
    }
    return 0;
}

void bgm_server(void) {
    if (*(u16 *)(game_w + 0x26) > 0) {
        *(u16 *)(game_w + 0x26) = *(u16 *)(game_w + 0x26) - 1;
    }
    if (game_w[0x10] & 2) {
        if (str_getstat(1) >= 4) {
            if (game_w[0x11] != 0) {
                str_fadein_vol(0, 0xF, 0x7F);
            } else {
                str_fadein_vol(0, 0xF, Snd_bgm_tbl[1 + game_w[0x14] * 2]);
            }
            game_w[0x10] = game_w[0x10] & 0xFD;
        }
    }
    if (game_w[0x11] < 7) {
        int r = Quest_clear_ck(1);
        if (r != 0) {
            if (r == 1) {
                if (*(s32 *)(quest_w + 0x40) & 2) {
                    game_w[0x11] = 7;
                    *(u16 *)(game_w + 0x26) = 0x50;
                } else {
                    str_fadeout(0, 0xF);
                    *(u16 *)(game_w + 0x26) = 0xF;
                    game_w[0x11] = 8;
                }
            } else {
                str_fadeout(0, 0xF);
                *(u16 *)(game_w + 0x26) = 0xF;
                game_w[0x11] = 10;
            }
        }
    }
    switch (game_w[0x11]) {
    case 0:
        if (em_status_ck() != 0) {
            game_w[0x11]++;
            str_fadeout(0, 0xF);
            *(u16 *)(game_w + 0x26) = 0xF;
        }
        break;
    case 1:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        game_w[0x11]++;
        *(u16 *)(game_w + 0x26) = 0x96;
        str_play_f_vol(0, 0x56, 0xF, 0x7F);
        game_w[0x25] = 0x56;
        break;
    case 2:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        switch (em_status_ck()) {
        case 0:
            game_w[0x11] = 5;
            str_fadeout(0, 0xF);
            break;
        case 1:
            break;
        case 2:
            game_w[0x11]++;
            str_fadeout(0, 0xF);
            *(u16 *)(game_w + 0x26) = 0xF;
            break;
        }
        break;
    case 3:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        game_w[0x11]++;
        *(u16 *)(game_w + 0x26) = 0x96;
        fight_bgm_set();
        break;
    case 4:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        switch (em_status_ck()) {
        case 0:
            *(u16 *)(game_w + 0x26) = 0xF;
            str_fadeout(0, 0xF);
            game_w[0x11]++;
            break;
        case 1:
            break;
        case 2:
            break;
        }
        break;
    case 5:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        game_w[0x11] = 0;
        stage_bgm_set(game_w[0x14]);
        break;
    case 7:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        str_fadeout(0, 0xF);
        *(u16 *)(game_w + 0x26) = 0xF;
        game_w[0x11] = 8;
    case 8:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        game_w[0x11]++;
        str_play_f_vol(0, 0x49, 0xF, 0x7F);
        game_w[0x25] = 0x49;
    case 9:
        if (Quest_clear_ck(1) == -1) {
            game_w[0x11] = 0xB;
            str_play_f_vol(0, 0x57, 0xF, 0x7F);
            game_w[0x25] = 0x57;
        }
        break;
    case 10:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        game_w[0x11]++;
        str_play_f_vol(0, 0x57, 0xF, 0x7F);
        game_w[0x25] = 0x57;
    case 11:
        if (Quest_clear_ck(1) == 1) {
            game_w[0x11] = 9;
            str_play_f_vol(0, 0x49, 0xF, 0x7F);
            game_w[0x25] = 0x49;
        }
        break;
    }
}

void adx_se_set(int a0, int id) {  /* PC: Pl_master_ck gets a0 (the player), as on the PS2 where a0 is left over */
    if (Pl_master_ck(a0) == 1) {
        str_play(1, id);
        *(s16 *)(game_w + 0x1E0) = id;
        if (game_w[0x11] != 0 && Quest_clear_ck(1) == 0) {
            game_w[0x10] = game_w[0x10] | 2;
            str_fadein_vol(0, 0xF, 0x5F);
        }
    }
}

void adx_se_stop(void *pl) {
    if (Pl_master_ck(pl) == 1) {
        if (*(u16 *)(game_w + 0x1E0) == 0xA) {
            str_stop(1, *(u16 *)(game_w + 0x1E0));
        }
        if (game_w[0x10] & 2) {
            game_w[0x10] = game_w[0x10] & 0xFD;
            if (game_w[0x11] == 0) {
                str_fadein_vol(0, 0xF, Snd_bgm_tbl[1 + game_w[0x14] * 2]);
                return;
            }
            str_fadein_vol(0, 0xF, 0x7F);
        }
    }
}

void die_bgm_set(void) {
    game_w[0x11] = 6;
    str_play_f_vol(0, 0x4A, 0xF, 0x7F);
}

/* PC: the 'same track, keep playing' branches also need the stream to be alive: vs_square_event (entering a house) calls str_stop_all just before, and without this the village stayed silent from then on */
int str_getstat(int);
void lobby_bgm_set(int n)
{
  int s2;
  u8 *t;
  u8 b;
  u8 *r;
  long long y;
  int x;
  if (((n & 0xFF) == 0x57) && (Quest_clear_bit_ck(0xAB) == 1))
  {
    if (game_w[0x25] == 0x1B && str_getstat(0) != 0)
    {
      str_pause(0, 0);
      str_volume(0, 0);
      str_fadein_vol(0, 0xF, Snd_bgm_tbl[1 + ((n & 0xFF) * 2)]);
    }
    else
    {
      str_play_f_vol(0, 0x1B, 0xF, Snd_bgm_tbl[1 + ((n & 0xFF) * 2)]);
    }
    game_w[0x25] = 0x1B;
  }
  else
  {
    s2 = (n & 0xFF) * 2;
    t = Snd_bgm_tbl + s2;
    b = t[0];
    if (game_w[0x25] == b && str_getstat(0) != 0)
    {
      str_pause(0, 0);
      str_volume(0, 0);
      str_fadein_vol(0, 0xF, Snd_bgm_tbl[1 + s2]);
    }
    else
    {
      str_play_f_vol(0, b, 0xF, Snd_bgm_tbl[1 + s2]);
    }
    game_w[0x25] = t[0];
  }
  r = Snd_rev_data_tbl + ((u8) n);
  y = Snd_rev_set_tbl[*r][1];
  x = Snd_rev_set_tbl[*r][0];
  flSndSetRev(0, x, y, 0, 0);
  y = Snd_rev_set_tbl[*r][3];
  x = Snd_rev_set_tbl[*r][2];
  flSndSetRev(1, x, y, 0, 0);
}

void lobby_bgm_set2(int n) {
    str_play(0, n & 0xFF);
    game_w[0x25] = n;
}

void fight_bgm_set(void) {
    u8 id;

    if (game_w[0x2E] >= 2 && game_w[0x2E] < 8) {
        id = fight_bgm_tbl[game_w[0x2E]];
    } else {
        switch (game_w[0x14]) {
        case 14:
            id = 0x4E;
            break;
        case 30:
            id = 0x4B;
            break;
        case 28:
            id = 0x4C;
            break;
        case 11:
            id = 0x4D;
            break;
        case 12:
            id = 0x47;
            break;
        default:
            id = 0x4E;
            break;
        }
    }
    str_play_f_vol(0, id, 0xF, 0x7F);
    game_w[0x25] = id;
}

int stage_bgm_etc_ck(int n) {
    u8 *p = stage_bgm_etc_tbl;

    while (*p != 0xFF) {
        if (*p == (n & 0xFF)) {
            return p[1];
        }
        p += 2;
    }
    return 0;
}

void demo_bgm_set(int n) {
    game_w[0x11] = 0;
    str_play_f_vol(0, n, 0xF, 0x7F);
    game_w[0x25] = n;
}

void stage_bgm_set(int n)
{
  int q;
  u8 *t;
  u8 b;
  int e;
  u8 *r;
  unsigned int y;
  int x;
  int s2;
  q = Quest_clear_ck(1);
  if (q != 0)
  {
    if (q == 1)
    {
      if (game_w[0x25] == 0x49)
      {
        str_pause(0, 0);
        str_volume(0, 0);
        str_fadein(0, 0xF);
      }
      else
      {
        game_w[0x11] = 9;
        str_play_f_vol(0, 0x49, 0xF, 0x7F);
        game_w[0x25] = 0x49;
      }
    }
    else
      if (game_w[0x25] == 0x57)
    {
      str_pause(0, 0);
      str_volume(0, 0);
      str_fadein(0, 0xF);
    }
    else
    {
      game_w[0x11] = 0xB;
      str_play_f_vol(0, 0x57, 0xF, 0x7F);
      game_w[0x25] = 0x57;
    }
  }
  else
  {
    if ((*((s32 *) (quest_w + 0x40))) & 0x800)
    {
      str_play_f_vol(0, 0x55, 0xF, 0x7F);
      game_w[0x25] = 0x55;
    }
    else
      if ((*((s32 *) (quest_w + 0x40))) & 0x400)
    {
      fight_bgm_set();
    }
    else
      if ((game_w[0x10] == 0) && ((e = stage_bgm_etc_ck(n)) != 0))
    {
      str_play_f_vol(0, e, 0xF, 0x7F);
      game_w[0x25] = e;
      game_w[0x10] = game_w[0x10] | 1;
    }
    else
    {
      s2 = (n & 0xFF) * 2;
      t = Snd_bgm_tbl + s2;
      b = t[0];
      if (game_w[0x25] == b)
      {
        str_pause(0, 0);
        str_volume(0, 0);
        str_fadein_vol(0, 0xF, Snd_bgm_tbl[1 + s2]);
      }
      else
      {
        str_play_f_vol(0, b, 0xF, Snd_bgm_tbl[1 + s2]);
      }
      game_w[0x25] = t[0];
    }
    game_w[0x11] = 0;
  }
  r = Snd_rev_data_tbl + (n & 0xFF);
  y = Snd_rev_set_tbl[*r][1];
  x = Snd_rev_set_tbl[*r][0];
  flSndSetRev(0, x, y, 0, 0);
  y = Snd_rev_set_tbl[*r][3];
  x = Snd_rev_set_tbl[*r][2];
  flSndSetRev(1, x, y, 0, 0);
}
