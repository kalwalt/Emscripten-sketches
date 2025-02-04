emcc simple_stb_example.cpp -I./stb_image -sWASM=1 -std=c++17 --embed-file chalk.jpg --bind -o stb_example.js

# to build with emcmake use these commands in a console
# cmake -G "Unix Makefiles" -B build -S ./ ..
# emmake make -C build