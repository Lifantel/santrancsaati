/*
 * display.h — Büyük ASCII rakamlarla terminal satranç saati görünümü
 */

#ifndef DISPLAY_H
#define DISPLAY_H

/* Terminali hazırlar: imleci gizler, boyutu ayarlamayı dener,
 * ekranı temizler. Program başında bir kez çağrılır. */
void display_init_terminal(void);

/* Terminali eski haline döndürür (imleci geri getirir). */
void display_restore_terminal(void);

/*
 * İki oyuncunun saatini ve durum mesajını ekrana çizer.
 *
 * p1_name / p2_name : oyuncu isimleri (NULL ise varsayılan kullanılır)
 * p1_seconds / p2_seconds : kalan süre (saniye, 0'ın altına inmez)
 * p1_active / p2_active   : o an sayaç işleyen taraf (1 = aktif)
 * status_msg : alt kısımda gösterilecek kısa durum/yardım metni (NULL olabilir)
 */
void display_clock(const char *p1_name, long p1_seconds, int p1_active,
                    const char *p2_name, long p2_seconds, int p2_active,
                    const char *status_msg);

#endif /* DISPLAY_H */
