emcc simple_stb_example.cpp -I./stb_image -sWASM=1 -std=c++17 --embed-file chalk.jpg --bind -o stb_example.js

# to build with emcmake use these commands in a console
# emcmake cmake -G "Unix Makefiles" -B build -S ./ ..
# emmake make -C build
# cp build/stb_example.js stb_example.js
# cp build/stb_example.wasm stb_example.wasm
# cp build/stb_example.data stb_example.data