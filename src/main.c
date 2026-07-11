#define _POSIX_C_SOURCE 200809L

/*
 * main.c — Satranc Saati (CLI)
 *
 * Kullanim:
 *   ./santrancsaat <zaman_kontrolu> [oyuncu1_adi] [oyuncu2_adi]
 *
 * Ornekler:
 *   ./santrancsaat 20+1
 *   ./santrancsaat 90sn+5 "Ali" "Veli"
 *
 * Tuslar:
 *   [SPACE]  Oyuncu 1 hamlesini bitirdi -> saat Oyuncu 2'ye gecer
 *   [ENTER]  Oyuncu 2 hamlesini bitirdi -> saat Oyuncu 1'e gecer
 *   [p]      duraklat / devam ettir
 *   [r]      sifirla
 *   [q]      cikis
 */

#include "kbhit.h"
#include "timectl.h"
#include "display.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static long now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

static void print_usage(const char *prog) {
    fprintf(stderr,
        "Kullanim: %s <zaman_kontrolu> [oyuncu1_adi] [oyuncu2_adi]\n\n"
        "Zaman kontrolu formati: <sure>[dk|sn][+<artis>[sn]]\n"
        "  Birim yazilmazsa ana sure dakika kabul edilir. Artis her zaman saniyedir.\n\n"
        "  Ornekler:\n"
        "    %s 20+1        -> 20 dakika, hamle basi +1 saniye\n"
        "    %s 20dk+1      -> ustekiyle ayni\n"
        "    %s 90sn+5      -> 90 saniye, hamle basi +5 saniye\n"
        "    %s 45sn        -> 45 saniye, artissiz\n"
        "    %s 10          -> 10 dakika, artissiz\n",
        prog, prog, prog, prog, prog, prog);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    TimeControl tc;
    if (!timectl_parse(argv[1], &tc)) {
        fprintf(stderr, "Hata: gecersiz zaman kontrolu formati: '%s'\n\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }

    const char *p1_name = (argc > 2) ? argv[2] : "Oyuncu 1";
    const char *p2_name = (argc > 3) ? argv[3] : "Oyuncu 2";

    long p1_ms = tc.base_seconds * 1000L;
    long p2_ms = tc.base_seconds * 1000L;
    long inc_ms = tc.increment_seconds * 1000L;

    /* active: -1 = henuz baslamadi, 0 = Oyuncu1'in saati isliyor, 1 = Oyuncu2'nin saati isliyor */
    int active = -1;
    int paused = 0;
    int game_over = 0;
    char status[160];
    snprintf(status, sizeof(status), "Baslamak icin bir tarafin tusuna basin.");

    kbhit_init();
    if (!kbhit_is_active()) {
        fprintf(stderr, "Hata: terminal (tty) bulunamadi. Bu program interaktif bir terminalde calistirilmali.\n");
        return 1;
    }
    display_init_terminal();

    long last_tick = now_ms();
    long last_draw = 0;

    while (1) {
        int c = kbhit_getch();

        if (c == 'q' || c == 'Q') {
            break;
        }

        if (!game_over) {
            if (c == 'r' || c == 'R') {
                p1_ms = tc.base_seconds * 1000L;
                p2_ms = tc.base_seconds * 1000L;
                active = -1;
                paused = 0;
                snprintf(status, sizeof(status), "Sifirlandi. Baslamak icin bir tarafin tusuna basin.");
            } else if (c == 'p' || c == 'P') {
                if (active != -1) {
                    paused = !paused;
                    snprintf(status, sizeof(status), paused ? "DURAKLATILDI (devam icin 'p')" : "Devam ediyor.");
                }
            } else if (c == ' ') {
                if (active == -1) {
                    active = 1;
                    paused = 0;
                    snprintf(status, sizeof(status), "Basladi. Sira: %s", p2_name);
                } else if (active == 0 && !paused) {
                    p1_ms += inc_ms;
                    active = 1;
                }
            } else if (c == '\n' || c == '\r') {
                if (active == -1) {
                    active = 0;
                    paused = 0;
                    snprintf(status, sizeof(status), "Basladi. Sira: %s", p1_name);
                } else if (active == 1 && !paused) {
                    p2_ms += inc_ms;
                    active = 0;
                }
            }
        }

        long t = now_ms();
        long elapsed = t - last_tick;
        last_tick = t;

        if (!game_over && !paused && active != -1) {
            if (active == 0) {
                p1_ms -= elapsed;
                if (p1_ms <= 0) {
                    p1_ms = 0;
                    game_over = 1;
                    snprintf(status, sizeof(status), "SURE BITTI! %s kaybetti.", p1_name);
                }
            } else {
                p2_ms -= elapsed;
                if (p2_ms <= 0) {
                    p2_ms = 0;
                    game_over = 1;
                    snprintf(status, sizeof(status), "SURE BITTI! %s kaybetti.", p2_name);
                }
            }
        }

        if (t - last_draw >= 150 || c != -1) {
            display_clock(p1_name, p1_ms / 1000, active == 0,
                          p2_name, p2_ms / 1000, active == 1,
                          status);
            last_draw = t;
        }

        struct timespec req = { 0, 20L * 1000L * 1000L }; /* ~20ms */
        nanosleep(&req, NULL);
    }

    display_restore_terminal();
    kbhit_cleanup();
    return 0;
}
