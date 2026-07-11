
> [!WARNING]
> Bu proje ihtiyaçlarım doğrultusunda yapay zekaya yazdırdığım bir proje her şeyin çalıştığına tam garanti veremem.

# santrancsaat

Terminalde çalışan, büyük ASCII rakamlarla süre gösteren satranç saati.
C ile yazıldı, harici bağımlılığı yok (sadece POSIX/libc).

## Derleme

```sh
make
```

`santrancsaat` adında bir çalıştırılabilir dosya üretir.

## Kullanım

```sh
./santrancsaat <zaman_kontrolu> [oyuncu1_adi] [oyuncu2_adi]
```

Zaman kontrolü **sabit kalıplara** (1+0, 3+2, 5+3 ...) bağlı değildir;
istediğin süreyi ve artışı serbestçe yazabilirsin:

```
<sure>[dk|sn][+<artis>[sn]]
```

- Birim yazılmazsa ana süre **dakika** kabul edilir.
- Artış (increment) her zaman **saniye**dir.

Örnekler:

```sh
./santrancsaat 20+1              # 20 dakika, hamle başı +1 saniye
./santrancsaat 20dk+1            # üsttekiyle aynı
./santrancsaat 90sn+5            # 90 saniye, hamle başı +5 saniye
./santrancsaat 45sn              # 45 saniye, artışsız
./santrancsaat 10                # 10 dakika, artışsız
./santrancsaat 15+10 "Ali" "Veli"
```

## Tuşlar

| Tuş     | Etki                                              |
|---------|----------------------------------------------------|
| `SPACE` | Oyuncu 1 hamlesini bitirdi → saat Oyuncu 2'ye geçer |
| `ENTER` | Oyuncu 2 hamlesini bitirdi → saat Oyuncu 1'e geçer  |
| `p`     | Duraklat / devam ettir                             |
| `r`     | Sıfırla                                            |
| `q`     | Çıkış                                              |

Oyun, taraflardan biri ilk kez tuşuna basana kadar başlamaz (o an
**karşı** oyuncunun saati işlemeye başlar — gerçek satranç saati mantığı:
kendi tuşuna basmak kendi saatini durdurup rakibinkini başlatır).

## Dosya yapısı

```
santrancsaat/
├── include/
│   ├── kbhit.h      # non-blocking klavye dinleme (senin kütüphanen)
│   ├── timectl.h     # zaman kontrolü string ayrıştırıcı
│   └── display.h     # ASCII rakam / ekran çizimi
├── src/
│   ├── kbhit.c
│   ├── timectl.c
│   ├── display.c
│   └── main.c        # ana döngü, saat mantığı
└── Makefile
```

## Notlar

- `kbhit_init()` içindeki `atexit()` sayesinde program beklenmedik
  şekilde sonlanırsa bile terminal ayarları geri yüklenir.
- Program başında bazı terminallerde (xterm ve türevleri) pencereyi
  orta boy bir alana (24x70) ayarlamayı dener (`\x1b[8;24;70t`).
  Bu, VT/xterm uzantısı olduğu için Konsole gibi bazı terminaller bunu
  görmezden gelebilir — böyle bir durumda terminal penceresini elle
  uygun boyuta getirmen yeterli.
- Ekran her karede tamamen temizlenmez; sadece satır satır
  `\x1b[K` ile üzerine yazılır, böylece titreme (flicker) azaltılır.
