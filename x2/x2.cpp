#include <emscripten.h>
extern "C" {
  int EMSCRIPTEN_KEEPALIVE inc(int a)
  {
      return ++a;
  }
}
