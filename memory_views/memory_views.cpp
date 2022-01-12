#include <emscripten/bind.h>
#include <emscripten/val.h>

unsigned char buffer[]=
{
   0x0,0xff,0xff,0xff,0x0,0x0,0x0,0xff,0xff,0xff,0x0,0x0,0x0,0xff,0xff,0xff,//*16bytes*
};
/* buffer is a 16 values buffer like:
[
  0, 255, 255, 255, 0, 0,
  0, 255, 255, 255, 0, 0,
  0, 255, 255, 255
]
*/
size_t bufferLength = 16;

emscripten::val getInt8Array() {
    return emscripten::val(
       emscripten::typed_memory_view(bufferLength,
                                     buffer));
}

EMSCRIPTEN_BINDINGS() {
    function("getInt8Array", &getInt8Array);
}