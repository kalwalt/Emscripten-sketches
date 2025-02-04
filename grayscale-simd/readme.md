# Grayscale SIMD Implementation

This folder contains two main implementations for converting video/images to grayscale using SIMD instructions:

## videoLuma.cpp
- Uses SIMD intrinsics (SSE2) for fast grayscale conversion
- Processes pixels in 16-byte chunks using 128-bit XMM registers
- Implements ITU-R BT.601 luma calculation: Y = 0.299R + 0.587G + 0.114B
- Optimized for performance by processing multiple pixels simultaneously
- Avoids using `_mm_hadd_epi32` for better performance ([see discussion](https://stackoverflow.com/questions/79397522/how-correctly-convert-mm-hadd-epi32-to-equivalent-c-code-wasm-simd128-h-wit))

## emscripten-gray.cpp
- WebAssembly/Emscripten version of grayscale conversion
- Utilizes SIMD.js for parallel processing
- Converts RGB/RGBA images to grayscale in the browser
- Compatible with web applications

Both implementations demonstrate significant performance improvements over scalar processing methods. For maximum performance, use the native `videoLuma.cpp` implementation when possible.

## Key Features

### emscripten-gray.cpp
- Contains C++ bindings for Emscripten integration
- Uses `EMSCRIPTEN_BINDINGS` to expose functions to JavaScript
- Allows direct function calls from web applications
- Enables seamless integration with browser-based workflows
