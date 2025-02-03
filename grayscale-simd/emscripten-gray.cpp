#include "videoLuma.cpp"
#include <chrono>
#include <iostream>
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

emscripten::val convert_to_luma(emscripten::val img, int width, int height, bool fastPath, bool simd128, int simdType)
{
    auto vli = arVideoLumaInit(width, height, fastPath, simd128, simdType);
    auto u8_img = emscripten::convertJSArrayToNumberVector<uint8_t>(img);

    std::chrono::high_resolution_clock::time_point duration_start = std::chrono::high_resolution_clock::now();

    auto out = arVideoLuma(vli, u8_img.data());

    std::chrono::high_resolution_clock::time_point duration_end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = (duration_end - duration_start);

    std::cout << "VideoLuma took " <<  duration.count() <<  "  seconds to execute\n";

    emscripten::val view{emscripten::typed_memory_view(width*height*1, out)};
    arVideoLumaFinal(&vli);
    auto result = emscripten::val::global("Uint8Array").new_(width*height*1);
    result.call<void>("set", view);
    return view;
}

EMSCRIPTEN_BINDINGS(gray_emscripten) {
    emscripten::function("convert_to_luma", &convert_to_luma);
}