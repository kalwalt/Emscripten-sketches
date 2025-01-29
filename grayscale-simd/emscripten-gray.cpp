#include "memory_simd_sse_threads.cpp"
#include "videoLuma.cpp"
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>


emscripten::val convert_to_gray(emscripten::val img, int width, int height, int channels, int threads) 
{
    auto u8_img = emscripten::convertJSArrayToNumberVector<uint8_t>(img);
    auto out = Rgb2Gray_useSSE(u8_img.data(), width, height);
    emscripten::val view{emscripten::typed_memory_view(width*height*1, out)};
    auto result = emscripten::val::global("Uint8Array").new_(width*height*1);
    result.call<void>("set", view);
    return view;
}

emscripten::val convert_to_luma(emscripten::val img, int width, int height, bool fastPath, bool simd128)
{
    auto vli = arVideoLumaInit(width, height, fastPath, simd128);
    auto u8_img = emscripten::convertJSArrayToNumberVector<uint8_t>(img);
    auto out = arVideoLuma(vli, u8_img.data());
    emscripten::val view{emscripten::typed_memory_view(width*height*1, out)};
    arVideoLumaFinal(&vli);
    auto result = emscripten::val::global("Uint8Array").new_(width*height*1);
    result.call<void>("set", view);
    return view;
}

EMSCRIPTEN_BINDINGS(gray_emscripten) {
    emscripten::function("convert_to_gray", &convert_to_gray);
    emscripten::function("convert_to_luma", &convert_to_luma);
}