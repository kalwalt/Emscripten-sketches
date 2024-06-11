em++ gray.cpp -mavx -msimd128 -sEXPORTED_FUNCTIONS=convert_memory_simd_sse -r -o gray.bc
# em++ gray.cpp -mavx -msimd128 -sEXPORTED_FUNCTIONS=convert_memory_simd_sse -s SAFE_HEAP=1 -gsource-map -sASSERTIONS=2 --profiling-funcs -r -o gray.bc
em++ gray.bc emscripten-gray.cpp -s ALLOW_MEMORY_GROWTH=1 -Oz --bind -o gray.js
# em++ gray.bc emscripten-gray.cpp -sEXPORTED_FUNCTIONS=convert_memory_simd_sse -s ALLOW_MEMORY_GROWTH=1 -s SAFE_HEAP=1 -gsource-map -sASSERTIONS=2 --profiling-funcs --bind -o gray.js
