/* sfd_test.c - standalone check of sfd.c (not part of the game build):
 *   sfd_test DISCDIR ENTRY OUT_PREFIX FIRST COUNT
 * decodes the movie in AFS00 entry ENTRY, writes frames FIRST..FIRST+COUNT-1
 * as raw yuv420p (OUT_PREFIX_NNNN.yuv, w*h Y then the two chroma planes) for
 * comparison with ffmpeg, and prints the decode time per frame.
 *   sfd_test DISCDIR ENTRY - 0 0 audio.wav   (audio check: see below) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sfd.h"

int main(int argc, char **argv)
{
    char path[1024];
    fmt_afs afs;
    sfd *s;
    int first, count, n = 0, w, h, fr;
    double tot, mx;
    if (argc < 6) {
        fprintf(stderr, "usage: sfd_test DISCDIR ENTRY PREFIX FIRST COUNT\n");
        return 2;
    }
    snprintf(path, sizeof path, "%s/AFS00.AFS", argv[1]);
    if (fmt_afs_open(&afs, path)) { fprintf(stderr, "no %s\n", path); return 1; }
    s = sfd_open(&afs, atoi(argv[2]));
    if (!s) { fprintf(stderr, "not a movie\n"); return 1; }
    if (argc >= 7 && !strcmp(argv[3], "-")) {           /* audio: the first COUNT seconds as raw s16 stereo */
        int16_t buf[32 * 2 * 64];
        FILE *f = fopen(argv[6], "wb");
        long want, got = 0;
        sfd_audio_fill(s, 4096);
        want = (long)atoi(argv[5]) * sfd_audio_rate(s);
        while (got < want) {
            int k;
            sfd_audio_fill(s, 1 << 16);
            k = sfd_audio_pull(s, buf, 64 * 32);
            if (k <= 0)
                break;
            fwrite(buf, 4, (size_t)k, f);
            got += k;
        }
        fclose(f);
        printf("audio: %ld frames at %d Hz, %d ch\n", got, sfd_audio_rate(s), sfd_audio_channels(s));
        return 0;
    }
    first = atoi(argv[4]);
    count = atoi(argv[5]);
    while (n < first + count && sfd_next_frame(s)) {
        if (n >= first) {
            const uint8_t *p[3];
            char name[1100];
            FILE *f;
            sfd_planes(s, p, &w, &h);
            snprintf(name, sizeof name, "%s_%04d.yuv", argv[3], n);
            f = fopen(name, "wb");
            fwrite(p[0], 1, (size_t)w * h, f);
            fwrite(p[1], 1, (size_t)w * h / 4, f);
            fwrite(p[2], 1, (size_t)w * h / 4, f);
            fclose(f);
        }
        n++;
    }
    sfd_stats(s, &fr, &tot, &mx);
    printf("%d frames decoded, %.2f ms per frame average, %.2f ms worst (%dx%d, %.3f fps)\n", fr, tot / (fr ? fr : 1), mx, w, h, sfd_fps(s));
    sfd_close(s);
    return 0;
}
