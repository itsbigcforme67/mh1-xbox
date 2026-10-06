/* bgm04 - f_bgm 0x0021DF80-0x0021E268: stage_bgm_set. Whole file in bgm_nm.c. */
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
