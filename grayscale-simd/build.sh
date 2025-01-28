emcc -I./ -mavx -mssse3 -msse4.1 -msse4.2 -msimd128 emscripten-gray.cpp -s ALLOW_MEMORY_GROWTH=1 -Oz -sWASM=1 --bind -o gray.js

# run this for debug infos
# emcc -I./ -mavx -msimd128 emscripten-gray.cpp -s ALLOW_MEMORY_GROWTH=1 -Oz -sWASM=1 --bind -g -sASSERTIONS=2 -o gray.js