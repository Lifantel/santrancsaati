/*
 * timectl.c — Satranç saati zaman kontrolü ayrıştırıcısı
 */

#include "timectl.h"

#include <ctype.h>
#include <stddef.h>

/* Bir işaretçiden başlayarak ondalıksız pozitif tamsayı okur.
 * Okunan basamak sayısı kadar *p'yi ilerletir.
 * Dönüş: en az 1 basamak okunduysa 1, hiç basamak yoksa 0. */
static int read_uint(const char **p, long *out) {
    const char *s = *p;
    if (!isdigit((unsigned char)*s)) return 0;

    long val = 0;
    while (isdigit((unsigned char)*s)) {
        val = val * 10 + (*s - '0');
        s++;
    }

    *out = val;
    *p = s;
    return 1;
}

/* "dk" veya "sn" birimini okur (varsa). Yoksa etkisiz döner.
 * unit_out: 'd' = dakika, 's' = saniye, 0 = belirtilmemiş */
static void read_unit(const char **p, char *unit_out) {
    const char *s = *p;
    if (s[0] == 'd' && s[1] == 'k') {
        *unit_out = 'd';
        *p = s + 2;
    } else if (s[0] == 's' && s[1] == 'n') {
        *unit_out = 's';
        *p = s + 2;
    } else {
        *unit_out = 0;
    }
}

int timectl_parse(const char *str, TimeControl *out) {
    if (str == NULL || out == NULL) return 0;

    const char *p = str;

    /* --- Ana süre --- */
    long base_val;
    if (!read_uint(&p, &base_val)) return 0;
    if (base_val <= 0) return 0;

    char base_unit;
    read_unit(&p, &base_unit);
    if (base_unit == 0) base_unit = 'd'; /* varsayılan: dakika */

    long base_seconds = (base_unit == 'd') ? base_val * 60 : base_val;

    /* --- Artış (opsiyonel) --- */
    long inc_seconds = 0;

    if (*p == '+') {
        p++;
        long inc_val;
        if (!read_uint(&p, &inc_val)) return 0; /* '+' varsa sayı zorunlu */

        char inc_unit;
        read_unit(&p, &inc_unit);
        /* Artış her zaman saniyedir; "dk" yazılması anlamsızdır. */
        if (inc_unit == 'd') return 0;

        inc_seconds = inc_val;
    }

    /* Fazladan karakter kalmışsa format hatalıdır. */
    if (*p != '\0') return 0;

    out->base_seconds = base_seconds;
    out->increment_seconds = inc_seconds;
    return 1;
}
