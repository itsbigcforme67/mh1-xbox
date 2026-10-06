/* bgm03 - f_bgm 0x0021DBE0-0x0021DDC0: lobby_bgm_set. Whole file in bgm_nm.c. */
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
    if (game_w[0x25] == 0x1B)
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
  r = Snd_rev_data_tbl + ((u8) n);
  y = Snd_rev_set_tbl[*r][1];
  x = Snd_rev_set_tbl[*r][0];
  flSndSetRev(0, x, y, 0, 0);
  y = Snd_rev_set_tbl[*r][3];
  x = Snd_rev_set_tbl[*r][2];
  flSndSetRev(1, x, y, 0, 0);
}
