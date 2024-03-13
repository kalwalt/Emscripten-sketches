# FMT example
## Build infos
### compile with docker and emcc
docker exec fmt emcc -I./src/fmt/include -s EXIT_RUNTIME=1 -o fmt.js ./src/fmt_example.cpp 

### compile with docker and emcmake

docker exec emcmake cmake -B build -S ./
docker exec fmt emmake make -C build