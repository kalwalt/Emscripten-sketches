# Change the libwebp path...
LIBWEBP=../libwebp
emcc -O3 -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' \
    -I $LIBWEBP \
    webp_start.c \
    $LIBWEBP/src/{dec,dsp,demux,enc,mux,utils}/*.c

emcc -O3 -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' \
    -I $LIBWEBP \
    webp.c \
    $LIBWEBP/src/{dec,dsp,demux,enc,mux,utils}/*.c -s ALLOW_MEMORY_GROWTH=1 -o webp_encoder.js