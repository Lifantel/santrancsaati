/*
 * kbhit.h — Non-blocking klavye dinleme kütüphanesi (POSIX/Linux)
 * 
 * 
 * (Bu kütüphane ile alakalı hiçbir sorumluluk almıyorum.)
 * (Bu kütüphaneyi belki ileride işime yarayacağını düşündüğüm için açıklamalarda yapay zeka kullanıldı.)
 * 
 * 
 * KULLANIM:
 *   1. kbhit_init()     programın başında bir kez çağır
 *   2. kbhit()          tuş var mı? (1=evet, 0=hayır)
 *   3. kbhit_getch()    varsa oku (-1 = boş)
 *   4. kbhit_cleanup()  programdan çıkmadan önce çağır
 *
 * Derleme:
 *   gcc -c kbhit.c -o kbhit.o
 *   gcc main.c kbhit.o -o program
 *
 * Veya tek seferde:
 *   gcc main.c kbhit.c -o program
 */

#ifndef KBHIT_H
#define KBHIT_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Terminal raw modunu etkinleştirir (ICANON ve ECHO kapatılır).
 * Kullanmadan önce bir kez çağrılmalı.
 * NOT: atexit() ile kbhit_cleanup() otomatik kaydedilir,
 *      manuel çağırmak gerekmez ama yine de tavsiye edilir.
 */
void kbhit_init(void);

/*
 * Terminal ayarlarını orijinal haline döndürür.
 * Program sonunda çağırılmalı.
 * kbhit_init() çağrılmamışsa etki etmez.
 */
void kbhit_cleanup(void);

/*
 * Klavyede okunmayı bekleyen bir tuş olup olmadığını kontrol eder.
 * Bloklamaz (non-blocking).
 * Dönüş: 1 = tuş var, 0 = yok, -1 = hata
 * Önkoşul: kbhit_init() çağrılmış olmalı.
 */
int kbhit(void);

/*
 * Bekleyen bir karakter varsa okuyup döner, yoksa -1 döner.
 * Bloklamaz (non-blocking).
 * Önkoşul: kbhit_init() çağrılmış olmalı.
 */
int kbhit_getch(void);

/*
 * kbhit_init() çağrılmış mı?
 * Dönüş: 1 = evet, 0 = hayır
 */
int kbhit_is_active(void);

#ifdef __cplusplus
}
#endif

#endif /* KBHIT_H */
