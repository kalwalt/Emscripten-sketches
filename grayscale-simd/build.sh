emcc gray.cpp -mavx -msimd128 -sEXPORTED_FUNCTIONS=convert_memory_simd_sse -r -o gray.bc
emcc gray.bc emscripten-gray.cpp -s ALLOW_MEMORY_GROWTH=1 -O3 --bind -o gray.js