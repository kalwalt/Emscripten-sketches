/*
 * Copyright 2014 The Emscripten Authors.  All rights reserved.
 * Emscripten is available under two separate licenses, the MIT license and the
 * University of Illinois/NCSA Open Source License.  Both these licenses can be
 * found in the LICENSE file.
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <emscripten.h>

int result = 0;

#define REPORT_RESULT(result) ({ if (result == 0) { printf("Test successful!\n");} else {printf("Test failed!\n");}})

void one() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_TEXTEDITING: assert(0); break;
      case SDL_TEXTINPUT:
        printf("Received %s\n", event.text.text);
        if (!strcmp("a", event.text.text)) {
          result = 1;
        } else if (!strcmp("A", event.text.text)) {
          REPORT_RESULT(result);
          emscripten_run_script("throw 'done'");
        }
        break;
    }
  }
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window;
  SDL_CreateWindow("window", 0, 0, 600, 450, 0);
  SDL_StartTextInput();

  emscripten_run_script("simulateKeyEvent('a'.charCodeAt(0))"); // a
  emscripten_run_script("simulateKeyEvent('A'.charCodeAt(0))"); // A

  one();

  return 0;
}
