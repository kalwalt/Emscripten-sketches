emcc test_fopen_write.cpp -o test_fopen_write.js
emcc test_readFile.cpp -o test_readFile.js -s EXPORTED_RUNTIME_METHODS="['FS']" -s FORCE_FILESYSTEM=1 -lnodefs.js
emcc test_write.cpp -o test_write.js -s EXPORTED_RUNTIME_METHODS="['FS']"
emcc test_writeFile.cpp -o test_writeFile.js -s EXPORTED_RUNTIME_METHODS="['FS']"
