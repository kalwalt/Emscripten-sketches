emcc gray.cpp -I./ -mavx -msimd128 -sEXPORTED_FUNCTIONS=convert_memory_simd_sse -r -o gray.bc
emcc gray.bc emscripten-gray.cpp -s ALLOW_MEMORY_GROWTH=1 -Oz --bind -o gray.js

#final correct build command:
emcc -I./ -pthread -mavx -msimd128 emscripten-gray.cpp -s ALLOW_MEMORY_GROWTH=1 -Oz -sWASM=1 --bind -o gray.js