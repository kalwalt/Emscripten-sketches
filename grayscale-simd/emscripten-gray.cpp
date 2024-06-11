#include "gray.h"
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>


emscripten::val convert_to_gray(emscripten::val img, int width, int height, int channels, int threads) 
{
    auto u8_img = emscripten::convertJSArrayToNumberVector<uint8_t>(img);
    auto out = convert_memory_simd_sse(u8_img.data(), width, height, channels, threads);
    emscripten::val view{emscripten::typed_memory_view(width*height*1, out)};
    auto result = emscripten::val::global("Uint8Array").new_(width*height*1);
    result.call<void>("set", view);
    return view;
}

EMSCRIPTEN_BINDINGS(gray_emscripten) {
    emscripten::function("convert_to_gray", &convert_to_gray);
}