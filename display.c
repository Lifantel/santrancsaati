/*
 * display.c — Büyük ASCII rakamlarla terminal satranç saati görünümü
 *
 * Rakamlar klasik 7-segment gösterge mantığıyla, segment bitmask'inden
 * türetilerek çizilir (sabit string tablosu yerine).
 *
 *      _a_
 *   f |   | b
 *      -g-
 *   e |   | c
 *      -d-
 */

#include "display.h"

#include <stdio.h>
#include <string.h>

#define SEG_A 0x01
#define SEG_B 0x02
#define SEG_C 0x04
#define SEG_D 0x08
#define SEG_E 0x10
#define SEG_F 0x20
#define SEG_G 0x40

static const unsigned char DIGIT_SEG[10] = {
    /* 0 */ SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,
    /* 1 */ SEG_B|SEG_C,
    /* 2 */ SEG_A|SEG_B|SEG_G|SEG_E|SEG_D,
    /* 3 */ SEG_A|SEG_B|SEG_G|SEG_C|SEG_D,
    /* 4 */ SEG_F|SEG_G|SEG_B|SEG_C,
    /* 5 */ SEG_A|SEG_F|SEG_G|SEG_C|SEG_D,
    /* 6 */ SEG_A|SEG_F|SEG_G|SEG_E|SEG_C|SEG_D,
    /* 7 */ SEG_A|SEG_B|SEG_C,
    /* 8 */ SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,
    /* 9 */ SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G,
};

#define GLYPH_ROWS   5
#define GLYPH_WIDTH  3   /* her rakam hücresi 3 karakter genişliğinde  */
#define CLOCK_GLYPHS 5   /* D D : D D                                  */
#define CLOCK_WIDTH  (GLYPH_WIDTH * CLOCK_GLYPHS)  /* 15 */
#define CONTENT_WIDTH (CLOCK_WIDTH + 2)            /* 17, kenarlarda 1'er boşluk payı */
#define BOX_WIDTH     (CONTENT_WIDTH + 2)          /* 19, sol/sağ çerçeve karakteri   */
#define BOX_HEIGHT    9  /* isim + boş + 5 rakam satırı + boş + durum satırı */

/* Tek bir rakamı 5 satırlık 3 karakterlik hücrelere çizer. */
static void render_digit(int d, char rows[GLYPH_ROWS][GLYPH_WIDTH + 1]) {
    unsigned char seg = DIGIT_SEG[d];

    snprintf(rows[0], GLYPH_WIDTH + 1, "%s", (seg & SEG_A) ? " _ " : "   ");
    rows[1][0] = (seg & SEG_F) ? '|' : ' ';
    rows[1][1] = ' ';
    rows[1][2] = (seg & SEG_B) ? '|' : ' ';
    rows[1][3] = '\0';
    snprintf(rows[2], GLYPH_WIDTH + 1, "%s", (seg & SEG_G) ? " _ " : "   ");
    rows[3][0] = (seg & SEG_E) ? '|' : ' ';
    rows[3][1] = ' ';
    rows[3][2] = (seg & SEG_C) ? '|' : ' ';
    rows[3][3] = '\0';
    snprintf(rows[4], GLYPH_WIDTH + 1, "%s", (seg & SEG_D) ? " _ " : "   ");
}

/* İki nokta üst üste (":") hücresini çizer, rakamlarla aynı hizada. */
static void render_colon(char rows[GLYPH_ROWS][GLYPH_WIDTH + 1]) {
    snprintf(rows[0], GLYPH_WIDTH + 1, "   ");
    snprintf(rows[1], GLYPH_WIDTH + 1, " * ");
    snprintf(rows[2], GLYPH_WIDTH + 1, "   ");
    snprintf(rows[3], GLYPH_WIDTH + 1, " * ");
    snprintf(rows[4], GLYPH_WIDTH + 1, "   ");
}

/* saniye değerini "MM:SS" büyük rakamlarına çevirir (5 satır). */
static void build_clock_glyph_lines(long seconds, char out[GLYPH_ROWS][CLOCK_WIDTH + 1]) {
    if (seconds < 0) seconds = 0;
    long mm = seconds / 60;
    long ss = seconds % 60;
    if (mm > 99) mm = 99; /* iki haneyle sınırlı gösterim */

    int digits[4] = { (int)(mm / 10), (int)(mm % 10), (int)(ss / 10), (int)(ss % 10) };

    char g0[GLYPH_ROWS][GLYPH_WIDTH + 1];
    char g1[GLYPH_ROWS][GLYPH_WIDTH + 1];
    char gc[GLYPH_ROWS][GLYPH_WIDTH + 1];
    char g2[GLYPH_ROWS][GLYPH_WIDTH + 1];
    char g3[GLYPH_ROWS][GLYPH_WIDTH + 1];

    render_digit(digits[0], g0);
    render_digit(digits[1], g1);
    render_colon(gc);
    render_digit(digits[2], g2);
    render_digit(digits[3], g3);

    for (int i = 0; i < GLYPH_ROWS; i++) {
        snprintf(out[i], CLOCK_WIDTH + 1, "%s%s%s%s%s", g0[i], g1[i], gc[i], g2[i], g3[i]);
    }
}

/* Metni verilen genişlikte ortalar. out en az width+1 boyutunda olmalı. */
static void center_text(const char *text, int width, char *out) {
    int len = (int)strlen(text);
    if (len > width) len = width;
    int left = (width - len) / 2;
    int right = width - len - left;
    int pos = 0;
    for (int i = 0; i < left; i++) out[pos++] = ' ';
    memcpy(out + pos, text, len);
    pos += len;
    for (int i = 0; i < right; i++) out[pos++] = ' ';
    out[pos] = '\0';
}

/* Bir oyuncunun kutusunu (çerçeve + isim + büyük rakamlar + durum) doldurur. */
static void build_player_box(char lines[BOX_HEIGHT + 2][96], const char *name,
                              long seconds, int active) {
    const char *reset = "\x1b[0m";
    const char *style = active ? "\x1b[1;32m" /* aktif: parlak yeşil */
                                : "\x1b[2m";   /* pasif: soluk        */

    char hline[CONTENT_WIDTH + 1];
    memset(hline, '-', CONTENT_WIDTH);
    hline[CONTENT_WIDTH] = '\0';

    int idx = 0;
    snprintf(lines[idx++], 96, "%s+%s+%s", style, hline, reset);

    char namebuf[CONTENT_WIDTH + 1];
    char namesrc[32];
    snprintf(namesrc, sizeof(namesrc), "%s%s", name, active ? " (*)" : "");
    center_text(namesrc, CONTENT_WIDTH, namebuf);
    snprintf(lines[idx++], 96, "%s|%s|%s", style, namebuf, reset);

    char blank[CONTENT_WIDTH + 1];
    memset(blank, ' ', CONTENT_WIDTH);
    blank[CONTENT_WIDTH] = '\0';
    snprintf(lines[idx++], 96, "%s|%s|%s", style, blank, reset);

    char glyphs[GLYPH_ROWS][CLOCK_WIDTH + 1];
    build_clock_glyph_lines(seconds, glyphs);
    for (int i = 0; i < GLYPH_ROWS; i++) {
        char row[CONTENT_WIDTH + 1];
        center_text(glyphs[i], CONTENT_WIDTH, row);
        snprintf(lines[idx++], 96, "%s|%s|%s", style, row, reset);
    }

    snprintf(lines[idx++], 96, "%s|%s|%s", style, blank, reset);

    char statusbuf[CONTENT_WIDTH + 1];
    center_text(active ? "CALISIYOR" : "bekliyor", CONTENT_WIDTH, statusbuf);
    snprintf(lines[idx++], 96, "%s|%s|%s", style, statusbuf, reset);

    snprintf(lines[idx++], 96, "%s+%s+%s", style, hline, reset);
}

void display_init_terminal(void) {
    /* İmleci gizle */
    printf("\x1b[?25l");
    /* Bazı terminallerde (xterm ve türevleri) pencereyi orta boy bir
     * alana ayarlamayı dener; desteklemeyen terminallerde etkisizdir. */
    printf("\x1b[8;24;70t");
    /* Ekranı bir kere temizle */
    printf("\x1b[2J\x1b[H");
    fflush(stdout);
}

void display_restore_terminal(void) {
    printf("\x1b[0m");    /* stil sıfırla       */
    printf("\x1b[?25h");  /* imleci geri getir  */
    printf("\n");
    fflush(stdout);
}

void display_clock(const char *p1_name, long p1_seconds, int p1_active,
                    const char *p2_name, long p2_seconds, int p2_active,
                    const char *status_msg) {
    char box1[BOX_HEIGHT + 2][96];
    char box2[BOX_HEIGHT + 2][96];

    build_player_box(box1, p1_name ? p1_name : "Oyuncu 1", p1_seconds, p1_active);
    build_player_box(box2, p2_name ? p2_name : "Oyuncu 2", p2_seconds, p2_active);

    /* İmleci başa al, her satırı 'satır sonuna kadar temizle' ile
     * yeniden yazarak titremeyi (flicker) azalt. */
    printf("\x1b[H");

    printf("\n  Satranc Saati\x1b[K\n\x1b[K\n");

    for (int i = 0; i < BOX_HEIGHT + 2; i++) {
        printf("  %s   %s\x1b[K\n", box1[i], box2[i]);
    }

    printf("\x1b[K\n");
    if (status_msg) {
        printf("  %s\x1b[K\n", status_msg);
    } else {
        printf("\x1b[K\n");
    }
    printf("\x1b[K\n");
    printf("  [SPACE] Oyuncu 1 hamle etti   [ENTER] Oyuncu 2 hamle etti\x1b[K\n");
    printf("  [p] duraklat/devam   [r] sifirla   [q] cikis\x1b[K\n");
    printf("\x1b[J");

    fflush(stdout);
}
