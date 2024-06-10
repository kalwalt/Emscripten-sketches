#include "gray.h"
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>


emscripten::val convert_to_gray(emscripten::val img, int width, int height, int channels, int threads) 
{
    auto u8_img = emscripten::convertJSArrayToNumberVector<uint8_t>(img);
    auto result = convert_memory_simd_sse(u8_img.data(), width, height, channels, threads);
    return emscripten::val(emscripten::typed_memory_view(width*height*1, result));
    delete[] result;
}

EMSCRIPTEN_BINDINGS(gray_emscripten) {
    emscripten::function("convert_to_gray", &convert_to_gray);
}