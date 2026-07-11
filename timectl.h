/*
 * timectl.h — Satranç saati zaman kontrolü ayrıştırıcısı
 *
 * Sabit kalıplar (1+0, 3+2, 5+3 ...) yerine serbest formatlı
 * bir zaman kontrolü string'i ayrıştırır.
 *
 * Desteklenen sözdizimi:
 *
 *   <sayı>[dk|sn]['+'<sayı>[sn]]
 *
 *   - Birinci sayı: ana süre. Birim belirtilmezse VARSAYILAN "dk" (dakika).
 *   - "dk" -> dakika, "sn" -> saniye
 *   - '+' sonrası: hamle başına eklenecek artış (Fischer increment),
 *     her zaman saniye cinsindendir. "sn" eki yazılsa da yazılmasa da olur.
 *   - '+' bölümü hiç yoksa artış 0 kabul edilir.
 *
 * Örnekler:
 *   "20+1"     -> 20 dakika, +1 saniye artış
 *   "20dk+1"   -> 20 dakika, +1 saniye artış   (üsttekiyle aynı)
 *   "90sn+5"   -> 90 saniye, +5 saniye artış
 *   "45sn"     -> 45 saniye, artışsız
 *   "10"       -> 10 dakika, artışsız
 *   "3dk+2sn"  -> 3 dakika, +2 saniye artış
 */

#ifndef TIMECTL_H
#define TIMECTL_H

typedef struct {
    long base_seconds;      /* oyuncu başına başlangıç süresi (saniye)   */
    long increment_seconds; /* her hamleden sonra eklenen süre (saniye)  */
} TimeControl;

/*
 * str içindeki zaman kontrolünü ayrıştırıp out'a yazar.
 * Dönüş: başarılıysa 1, format hatalıysa 0.
 * Hatalıysa out'un içeriği tanımsızdır.
 */
int timectl_parse(const char *str, TimeControl *out);

#endif /* TIMECTL_H */
