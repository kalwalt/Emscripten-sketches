//
// Created by @kalwalt on 28/01/2025.
//

#include <stdlib.h>
#include <wasm_simd128.h>

typedef unsigned char     ARUint8;
// CCIR 601 recommended values. See http://www.poynton.com/notes/colour_and_gamma/ColorFAQ.html#RTFToC11 .
const int R8_CCIR601 = 77;
const int G8_CCIR601 = 150;
const int B8_CCIR601 = 29;

struct ARVideoLumaInfo {
    int xsize;
    int ysize;
    int buffSize;
    int fastPath = 1;
    ARUint8 *__restrict buff;
};

static void arVideoLumaRGBAtoL_Emscripten_simd128(uint8_t * __restrict dest, uint8_t * __restrict src, int32_t numPixels);

ARVideoLumaInfo *arVideoLumaInit(int xsize, int ysize)
{
    ARVideoLumaInfo *vli;

    vli = (ARVideoLumaInfo *)calloc(1, sizeof(ARVideoLumaInfo));
    if (!vli) {
        printf("Out of memory!!\n");
        return (NULL);
    }
    vli->xsize = xsize;
    vli->ysize = ysize;
    vli->buffSize = xsize*ysize;
    vli->buff = (ARUint8 *)valloc(vli->buffSize);
    if (!vli->buff) {
        printf("Out of memory!!\n");
        free(vli);
        return (NULL);
    }

    return (vli);
}


ARUint8 *__restrict arVideoLuma(ARVideoLumaInfo *vli, const ARUint8 *__restrict dataPtr)
{
    unsigned int p, q;

    if (vli->fastPath) {

        arVideoLumaRGBAtoL_Emscripten_simd128(vli->buff, (unsigned char *__restrict)dataPtr, vli->buffSize);

        return (vli->buff);
    }
    q = 0;
    for (p = 0; p < ((unsigned int)vli->buffSize); p++) {
        vli->buff[p] = (R8_CCIR601*dataPtr[q + 0] + G8_CCIR601*dataPtr[q + 1] + B8_CCIR601*dataPtr[q + 2]) >> 8;
        q += 4;
    }
    return (vli->buff);
    }

static void arVideoLumaRGBAtoL_Emscripten_simd128(uint8_t * __restrict dest, uint8_t * __restrict src, int32_t numPixels) {
    v128_t RGBScale = wasm_i32x4_make(0, B8_CCIR601, G8_CCIR601, R8_CCIR601); // RGBScale = [0, B8_CCIR601, G8_CCIR601, R8_CCIR601]
    int numPixelsDiv8 = numPixels >> 3;

    for (int i = 0; i < numPixelsDiv8; i++) {
        v128_t pixels0_3 = wasm_v128_load(src); // Load 16 bytes (4 pixels) from src
        v128_t pixels4_7 = wasm_v128_load(src + 16); // Load next 16 bytes (4 pixels) from src

        // Unpack and interleave the low and high bytes of each 16-byte lane
        v128_t pixels0_3_l = wasm_i16x8_shuffle(pixels0_3, pixels0_3, 0, 2, 4, 6, 8, 10, 12, 14);
        v128_t pixels0_3_h = wasm_i16x8_shuffle(pixels0_3, pixels0_3, 1, 3, 5, 7, 9, 11, 13, 15);
        v128_t pixels4_7_l = wasm_i16x8_shuffle(pixels4_7, pixels4_7, 0, 2, 4, 6, 8, 10, 12, 14);
        v128_t pixels4_7_h = wasm_i16x8_shuffle(pixels4_7, pixels4_7, 1, 3, 5, 7, 9, 11, 13, 15);

        // Multiply and add the RGB components
        v128_t y0_3_l = wasm_i32x4_mul(pixels0_3_l, RGBScale);
        v128_t y0_3_h = wasm_i32x4_mul(pixels0_3_h, RGBScale);
        v128_t y4_7_l = wasm_i32x4_mul(pixels4_7_l, RGBScale);
        v128_t y4_7_h = wasm_i32x4_mul(pixels4_7_h, RGBScale);

        // Horizontal add the result
        y0_3_l = wasm_i32x4_add(y0_3_l, y0_3_h);
        y4_7_l = wasm_i32x4_add(y4_7_l, y4_7_h);

        // Shift right by 8 bits to divide by 256
        y0_3_l = wasm_u32x4_shr(y0_3_l, 8);
        y4_7_l = wasm_u32x4_shr(y4_7_l, 8);

        // Pack the 32-bit results into 16-bit and then into 8-bit values
        y0_3_l = wasm_i16x8_narrow_i32x4(y0_3_l, y0_3_l);
        y4_7_l = wasm_i16x8_narrow_i32x4(y4_7_l, y4_7_l);
        y0_3_l = wasm_i8x16_narrow_i16x8(y0_3_l, y0_3_l);
        y4_7_l = wasm_i8x16_narrow_i16x8(y4_7_l, y4_7_l);

        // Store the result back to dest
        wasm_v128_store(dest, y0_3_l);
        wasm_v128_store(dest + 16, y4_7_l);

        src += 32;
        dest += 8;
    }
}
