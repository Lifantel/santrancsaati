/*
 * kbhit.c — Non-blocking klavye dinleme kütüphanesi (POSIX/Linux)
 */

#include "kbhit.h"

#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>   /* atexit() */

/* İç durum*/

static struct termios _saved_term;   /* orijinal terminal ayarları    */
static int            _initialized = 0;


/* Yardımcı: atexit ile otomatik temizlik*/

static void _auto_cleanup(void) {
    kbhit_cleanup();
}


/* Genel API */


void kbhit_init(void) {
    if (_initialized) return;   /* çift init'e karşı koruma */

    /* Mevcut terminal ayarlarını kaydet */
    if (tcgetattr(STDIN_FILENO, &_saved_term) != 0)
        return;  /* tty değilse (pipe, redirect) sessizce çık */

    /* Raw mod: satır tamponu ve eko kapat */
    struct termios raw = _saved_term;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    _initialized = 1;

    /* Program herhangi bir şekilde biterse terminali kurtar */
    atexit(_auto_cleanup);
}

void kbhit_cleanup(void) {
    if (!_initialized) return;
    tcsetattr(STDIN_FILENO, TCSANOW, &_saved_term);
    _initialized = 0;
}

int kbhit_is_active(void) {
    return _initialized;
}

int kbhit(void) {
    if (!_initialized) return -1;   

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);

    struct timeval tv = {0, 0};     
    int ret = select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv);

    if (ret < 0) return -1;         
    return (ret > 0) ? 1 : 0;
}

int kbhit_getch(void) {
    if (kbhit() != 1) return -1;

    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) == 1)
        return (int)c;

    return -1;
}
