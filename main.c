#include <ctype.h>
#include <stdlib.h> //rozne taki z pamiecia
#include <termios.h> // terminal I/O interfaces
#include <stdio.h> // I/O api
#include <errno.h>
#include <unistd.h> // POSIX system API

#define CTRL_KEY(k) ((k) & 0x11)

struct termios orig_termios;

void die(const char *s) { //const means the fn wont modify s variable
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
    die("disabling raw mode failed");
  }
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
    die("enabling raw mode failed, start");
  }
   atexit(disableRawMode);
   
   struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); //co dochuja
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 10;

   if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    die("enabling raw mode failed, end");
   }
}

int main() {
  enableRawMode();
  while (1) {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
    // if (c == 'q') break;
    if (c == CTRL_KEY('q')) break;
  }
  return 0;
}
