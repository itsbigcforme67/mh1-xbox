/* Yes/No (network setup) overlay: sound effect helpers. yn.bin 0x0053B260-0x0053B2E4. */
void se_req(int, int, int);

void yn_cur1_sd(void) { se_req(7, 0x12, 0); }
void yn_cur2_sd(void) { se_req(7, 0x17, 0); }
void yn_dec_sd(void) { se_req(7, 0x13, 0); }
void yn_ng_sd(void) { se_req(7, 0x15, 0); }
void yn_can_sd(void) { se_req(7, 0x14, 0); }
void yn_log_sd(void) { se_req(7, 0x11, 0); }

void str_stop_all(void);
void se_stop_all(void);

void yn_stop_bgm(void) {
    str_stop_all();
    se_stop_all();
}
