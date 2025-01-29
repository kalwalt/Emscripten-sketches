//
// Created by @kalwalt on 28/01/2025.
//

#include <emmintrin.h> // SSE2.
#include <nmmintrin.h> // SSE4.2.
#include <pmmintrin.h> // SSE3.
#include <smmintrin.h> // SSE4.1.
#include <stdio.h>
#include <stdlib.h>
#include <tmmintrin.h> // SSSE3.
#include <wasm_simd128.h>

typedef unsigned char ARUint8;
// CCIR 601 recommended values. See
// http://www.poynton.com/notes/colour_and_gamma/ColorFAQ.html#RTFToC11 .
const uint8_t R8_CCIR601 = 77;
const uint8_t G8_CCIR601 = 150;
const uint8_t B8_CCIR601 = 29;

struct ARVideoLumaInfo {
  int xsize;
  int ysize;
  int buffSize;
  bool fastPath;
  bool simd128;
  ARUint8 *__restrict buff;
};

static void arVideoLumaRGBAtoL_Intel_simd_asm(uint8_t *__restrict dest,
                                              uint8_t *__restrict src,
                                              int32_t numPixels);
static void arVideoLumaRGBAtoL_Emscripten_simd128(uint8_t *__restrict dest,
                                                  uint8_t *__restrict src,
                                                  int32_t numPixels);

ARVideoLumaInfo *arVideoLumaInit(int xsize, int ysize, bool fastPath,
                                 bool simd128) {
  ARVideoLumaInfo *vli;

  vli = (ARVideoLumaInfo *)calloc(1, sizeof(ARVideoLumaInfo));
  if (!vli) {
    printf("Out of memory!!\n");
    return (NULL);
  }
  vli->xsize = xsize;
  vli->ysize = ysize;
  vli->buffSize = xsize * ysize;
  vli->fastPath = fastPath;
  vli->simd128 = simd128;
  vli->buff = (ARUint8 *)valloc(vli->buffSize);
  if (!vli->buff) {
    printf("Out of memory!!\n");
    free(vli);
    return (NULL);
  }

  return (vli);
}

ARUint8 *__restrict arVideoLuma(ARVideoLumaInfo *vli,
                                const ARUint8 *__restrict dataPtr) {
  unsigned int p, q;
  printf("vli->fastPath: %d\n", vli->fastPath);

  if (vli->fastPath == true) {
    printf("With fastPath!!!\n");
    if (vli->simd128 == true) {
      printf("With simd128!!!\n");
      arVideoLumaRGBAtoL_Emscripten_simd128(
          vli->buff, (unsigned char *__restrict)dataPtr, vli->buffSize);

      return (vli->buff);
    } else {
      printf("Without simd128!!!\n");
      arVideoLumaRGBAtoL_Intel_simd_asm(
          vli->buff, (unsigned char *__restrict)dataPtr, vli->buffSize);
      return (vli->buff);
    }
  } else {
    printf("default luma conversion!!!\n");
    q = 0;
    for (p = 0; p < ((unsigned int)vli->buffSize); p++) {
      vli->buff[p] =
          (R8_CCIR601 * dataPtr[q + 0] + G8_CCIR601 * dataPtr[q + 1] +
           B8_CCIR601 * dataPtr[q + 2]) >>
          8;
      q += 4;
    }
    return (vli->buff);
  }
}

int arVideoLumaFinal(ARVideoLumaInfo **vli_p) {
  if (!vli_p)
    return (-1);
  if (!*vli_p)
    return (0);

  free((*vli_p)->buff);
  free(*vli_p);
  *vli_p = NULL;

  return (0);
}

v128_t hadd_epi32(v128_t a, v128_t b) {
  v128_t shuffled_a = wasm_i32x4_shuffle(a, b, 2, 0, 2, 0);
  v128_t shuffled_b = wasm_i32x4_shuffle(a, b, 3, 1, 3, 1);
  return wasm_i32x4_add(shuffled_a, shuffled_b);
}

static void arVideoLumaRGBAtoL_Emscripten_simd128(uint8_t *__restrict dest,
                                                  uint8_t *__restrict src,
                                                  int32_t numPixels) {
  v128_t RGBScale = wasm_i16x8_make(
      0, B8_CCIR601, G8_CCIR601, R8_CCIR601, 0, B8_CCIR601, G8_CCIR601,
      R8_CCIR601); // RGBScale = [0, B8_CCIR601, G8_CCIR601, R8_CCIR601]
  int numPixelsDiv8 = numPixels >> 3;

  for (int i = 0; i < numPixelsDiv8; i++) {
    v128_t pixels0_3 = wasm_v128_load(src); // Load 16 bytes (4 pixels) from src
    v128_t pixels4_7 =
        wasm_v128_load(src + 16); // Load next 16 bytes (4 pixels) from src

    // Unpack and interleave the low and high bytes of each 16-byte lane
    v128_t pixels0_3_l =
        wasm_i16x8_shuffle(pixels0_3, pixels0_3, 0, 2, 4, 6, 8, 10, 12, 14);
    v128_t pixels0_3_h =
        wasm_i16x8_shuffle(pixels0_3, pixels0_3, 1, 3, 5, 7, 9, 11, 13, 15);
    v128_t pixels4_7_l =
        wasm_i16x8_shuffle(pixels4_7, pixels4_7, 0, 2, 4, 6, 8, 10, 12, 14);
    v128_t pixels4_7_h =
        wasm_i16x8_shuffle(pixels4_7, pixels4_7, 1, 3, 5, 7, 9, 11, 13, 15);

    // Multiply and add the RGB components
    v128_t y0_3_l = wasm_i32x4_dot_i16x8(pixels0_3_l, RGBScale);
    v128_t y0_3_h = wasm_i32x4_dot_i16x8(pixels0_3_h, RGBScale);
    v128_t y4_7_l = wasm_i32x4_dot_i16x8(pixels4_7_l, RGBScale);
    v128_t y4_7_h = wasm_i32x4_dot_i16x8(pixels4_7_h, RGBScale);

    // Horizontal add the result
    v128_t y0_3 = hadd_epi32(y0_3_l, y0_3_h);
    v128_t y4_7 = hadd_epi32(y4_7_l, y4_7_h);
    //v128_t y0_3 = wasm_i32x4_add(y0_3_l, y0_3_h);
    //v128_t y4_7 = wasm_i32x4_add(y4_7_l, y4_7_h);

    // Shift right by 8 bits to divide by 256
    y0_3 = wasm_u32x4_shr(y0_3, 8);
    y4_7 = wasm_u32x4_shr(y4_7, 8);

    // Pack the 32-bit results into 16-bit and then into 8-bit values
    y0_3 = wasm_i16x8_narrow_i32x4(y0_3, y0_3);
    y4_7 = wasm_i16x8_narrow_i32x4(y4_7, y4_7);
    y0_3 = wasm_u8x16_narrow_i16x8(y0_3, y0_3);
    y4_7 = wasm_u8x16_narrow_i16x8(y4_7, y4_7);

    // Store the result back to dest
    wasm_v128_store(dest, y0_3);
    wasm_v128_store(dest + 16, y4_7);

    src += 32;
    dest += 8;
  }
}
static void arVideoLumaRGBAtoL_Intel_simd_asm(uint8_t *__restrict dest,
                                              uint8_t *__restrict src,
                                              int32_t numPixels) {
  __m128i *pin = (__m128i *)src;
  uint32_t *pout = (uint32_t *)dest;
  int numPixelsDiv8 = numPixels >> 3;
  __m128i RGBScale = _mm_set_epi16(
      0, B8_CCIR601, G8_CCIR601, R8_CCIR601, 0, B8_CCIR601, G8_CCIR601,
      R8_CCIR601); // RGBScale =
                   // 000000[B8_CCIR601]00[G8_CCIR601]00[R8_CCIR601]000000[B8_CCIR601]00[G8_CCIR601]00[R8_CCIR601].

  do {
    __m128i pixels0_3 = _mm_load_si128(
        pin++); // pixels0_3 =
                // [A3][B3][G3][R3][A2][B2][G2][R2][A1][B1][G1][R1][A0][B0][G0][R0].
    __m128i pixels4_7 = _mm_load_si128(
        pin++); // pixels4_7 =
                // [A7][B7][G7][R7][A6][B6][G6][R6][A5][B5][G5][R5][A4][B4][G4][R4].

    __m128i pixels0_3_l = _mm_unpacklo_epi8(
        pixels0_3,
        _mm_setzero_si128()); // pixels0_3_l =
                              // 00[A1]00[B1]00[G1]00[R1]00[A0]00[B0]00[G0]00[R0].
    __m128i pixels0_3_h = _mm_unpackhi_epi8(
        pixels0_3,
        _mm_setzero_si128()); // pixels0_3_h =
                              // 00[A3]00[B3]00[G3]00[R3]00[A2]00[B2]00[G2]00[R2].
    __m128i pixels4_7_l = _mm_unpacklo_epi8(
        pixels4_7,
        _mm_setzero_si128()); // pixels4_7_l =
                              // 00[A5]00[B5]00[G5]00[R5]00[A4]00[B4]00[G4]00[R4].
    __m128i pixels4_7_h = _mm_unpackhi_epi8(
        pixels4_7,
        _mm_setzero_si128()); // pixels4_7_h =
                              // 00[A7]00[B7]00[G7]00[R7]00[A6]00[B6]00[G6]00[R6].

    __m128i y0_3_l = _mm_madd_epi16(pixels0_3_l, RGBScale);
    __m128i y0_3_h = _mm_madd_epi16(pixels0_3_h, RGBScale);
    __m128i y4_7_l = _mm_madd_epi16(pixels4_7_l, RGBScale);
    __m128i y4_7_h = _mm_madd_epi16(pixels4_7_h, RGBScale);
    __m128i y0_3 = _mm_hadd_epi32(y0_3_l, y0_3_h);
    __m128i y4_7 = _mm_hadd_epi32(y4_7_l, y4_7_h);

    y0_3 = _mm_srli_epi32(y0_3, 8);
    y4_7 = _mm_srli_epi32(y4_7, 8);
    y0_3 = _mm_packs_epi32(y0_3, y0_3);
    y4_7 = _mm_packs_epi32(y4_7, y4_7);
    y0_3 = _mm_packus_epi16(y0_3, y0_3);
    y4_7 = _mm_packus_epi16(y4_7, y4_7);

    *pout++ = _mm_cvtsi128_si32(y0_3);
    *pout++ = _mm_cvtsi128_si32(y4_7);

    numPixelsDiv8--;
  } while (numPixelsDiv8);
}
