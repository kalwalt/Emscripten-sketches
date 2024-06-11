em++ gray.cpp -mavx -msimd128 -sEXPORTED_FUNCTIONS=convert_memory_simd_sse -r -o gray.bc
em++ gray.bc emscripten-gray.cpp -s ALLOW_MEMORY_GROWTH=1 -Oz --bind -o gray.js