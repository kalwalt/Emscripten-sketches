#if defined __EMSCRIPTEN__
  #include <emscripten.h>
#endif
#include <stdio.h>
#include "log.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define Color_Red "\x1b[31m" // Color Start
#define Color_end "\x1b[0m" // To flush out prev settings
#define LOG_RED(X) printf("%s %s %s",Color_Red,X,Color_end)

#define BLUE(string) "\x1b[34m" string "\x1b[0m"
#define RED(string) "\x1b[31m" string "\x1b[0m"

int main(){
  ARLOGi("Hello this is a simple message!\n");
  ARLOGw("Hey this is a warning!\n");
  ARLOGe("This instead is an Error!!!\n");
  ARLOGd("Can we see this?\n");
  printf(ANSI_COLOR_RED     "This text is RED!"     ANSI_COLOR_RESET "\n");
  printf(ANSI_COLOR_GREEN   "This text is GREEN!"   ANSI_COLOR_RESET "\n");
  printf(ANSI_COLOR_YELLOW  "This text is YELLOW!"  ANSI_COLOR_RESET "\n");
  printf(ANSI_COLOR_BLUE    "This text is BLUE!"    ANSI_COLOR_RESET "\n");
  printf(ANSI_COLOR_MAGENTA "This text is MAGENTA!" ANSI_COLOR_RESET "\n");
  printf(ANSI_COLOR_CYAN    "This text is CYAN!"    ANSI_COLOR_RESET "\n");
  printf("\x1b[34mHello, world!\x1b[0m\n");
  LOG_RED("This is in Red Color\n");
  printf("This is " RED("red") "!\n");
  // a somewhat more complex ...
  printf("This is " BLUE("%s") "!\n","blue");
  printf("\x1b[104m %s\x1b[0m\n", "Hello blue!");
}