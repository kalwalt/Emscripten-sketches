/**
 * @file videoLuma.cpp
 * @brief RGBA to Luma conversion using SIMD instructions
 * @author @kalwalt
 * @date 28/01/2025
 * 
 * This code provides efficient RGBA to Luma color space conversion using SIMD instructions.
 * It supports both Intel SIMD (SSE) and WebAssembly SIMD128 implementations.
 * 
 * The conversion uses CCIR 601 recommended coefficients for RGB to Luma conversion:
 * L = 0.299R + 0.587G + 0.114B
 * (represented as fixed-point integers: 77R + 150G + 29B)
 * 
 * The code is partially derived from ARToolKit5's videoLuma.c:
 * @see https://github.com/artoolkit/ARToolKit5/blob/master/lib/SRC/Video/videoLuma.c
 * Modified to focus solely on RGBA to Luma conversion with SIMD optimization
 * (NEON code removed).
 * 
 * Several optimization paths are available:
 * - Standard non-SIMD conversion
 * - Intel SIMD (SSE) implementation
 * - WebAssembly SIMD128 implementation
 * - Fast path variants for both SIMD implementations
 * 
 * @note Requires SSE2, SSE3, SSSE3, SSE4.1, SSE4.2 support for Intel SIMD
 * @note Requires SIMD128 support for WebAssembly implementation
 */

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
  int simdType;
  ARUint8 *__restrict buff;
};

static void arVideoLumaRGBAtoL_Intel_simd_asm(uint8_t *__restrict dest,
                                              uint8_t *__restrict src,
                                              int32_t numPixels);
static void arVideoLumaRGBAtoL_Intel_simd_asm_w_mm_add_epi32( uint8_t* __restrict dest,
    uint8_t* __restrict src, int32_t numPixels );
static void arVideoLumaRGBAtoL_Emscripten_simd128(uint8_t *__restrict dest,
                                                  uint8_t *__restrict src,
                                                  int32_t numPixels);
static void arVideoLumaRGBAtoL_Emscripten_simd128_fast(uint8_t *__restrict dest,
                                                  uint8_t *__restrict src,
                                                  int32_t numPixels);

ARVideoLumaInfo *arVideoLumaInit(int xsize, int ysize, bool fastPath,
                                 bool simd128, int simdType) {
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
  vli->simdType = simdType;
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

  if (vli->fastPath == true) {
    printf("With fastPath!!!\n");
    if (vli->simd128 == true) {
      printf("With simd128!!!\n");
      if (vli->simdType == 1) {
        arVideoLumaRGBAtoL_Emscripten_simd128(
            vli->buff, (unsigned char *__restrict)dataPtr, vli->buffSize);
      } else if (vli->simdType == 2) {
        arVideoLumaRGBAtoL_Emscripten_simd128_fast(
            vli->buff, (unsigned char *__restrict)dataPtr, vli->buffSize);
      }
      return (vli->buff);
    } else {
      printf("Without simd128!!!\n");
      if (vli->simdType == 1) {
        arVideoLumaRGBAtoL_Intel_simd_asm(
            vli->buff, (unsigned char *__restrict)dataPtr, vli->buffSize);
      } else if (vli->simdType == 2) {
        arVideoLumaRGBAtoL_Intel_simd_asm_w_mm_add_epi32(
            vli->buff, (unsigned char *__restrict)dataPtr, vli->buffSize);
      }
      return (vli->buff);
    }
  } else {
    printf("default luma conversion!!!\n");
    q = 0;
    for (p = 0; p < ((unsigned int)vli->buffSize); p++) {
      vli->buff[p] =
          (R8_CCIR601 * dataPtr[q + 0] + G8_CCIR601 * dataPtr[q + 1] +
           B8_CCIR601 * dataPtr[q + 2]) >> 8;
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
  v128_t shuffled_a = wasm_i32x4_shuffle(a, b, 0, 2, 4, 6);
  v128_t shuffled_b = wasm_i32x4_shuffle(a, b, 1, 3, 5, 7);
  return wasm_i32x4_add(shuffled_a, shuffled_b);
}

v128_t unpacklo_epi8(v128_t a, v128_t b) {
  return wasm_i8x16_shuffle(a, b, 0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23);
}

v128_t unpackhi_epi8(v128_t a, v128_t b) {
  return wasm_i8x16_shuffle(a, b, 8, 24, 9, 25, 10, 26, 11, 27, 12, 28, 13, 29, 14, 30, 15, 31);
}

static void arVideoLumaRGBAtoL_Emscripten_simd128(uint8_t *__restrict dest,
                                                  uint8_t *__restrict src,
                                                  int32_t numPixels) {
  printf("using arVideoLumaRGBAtoL_Emscripten_simd128 !!!\n");
  v128_t *pin = (v128_t*)src;
  uint32_t *pout = (uint32_t *)dest;
  v128_t RGBScale = wasm_i16x8_make(
      0, B8_CCIR601, G8_CCIR601, R8_CCIR601, 0, B8_CCIR601, G8_CCIR601,
      R8_CCIR601); // RGBScale = [0, B8_CCIR601, G8_CCIR601, R8_CCIR601]
  int numPixelsDiv8 = numPixels >> 3;


    do {
    v128_t pixels0_3 = wasm_v128_load(pin++); // Load 16 bytes (4 pixels) from src
    v128_t pixels4_7 = wasm_v128_load(pin++); // Load next 16 bytes (4 pixels) from src
    // Unpack and interleave the low and high bytes of each 16-byte lane
    /*__m128i pixels0_3_l = _mm_unpacklo_epi8(
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
                              // 00[A7]00[B7]00[G7]00[R7]00[A6]00[B6]00[G6]00[R6].*/
    v128_t pixels0_3_l = unpacklo_epi8(pixels0_3, wasm_i64x2_const(0,0));
    v128_t pixels0_3_h = unpackhi_epi8(pixels0_3, wasm_i64x2_const(0,0));
    v128_t pixels4_7_l = unpacklo_epi8(pixels4_7, wasm_i64x2_const(0,0));
    v128_t pixels4_7_h = unpackhi_epi8(pixels4_7, wasm_i64x2_const(0,0));

    // Multiply and add the RGB components
    /*__m128i y0_3_l = _mm_madd_epi16(pixels0_3_l, RGBScale);
    __m128i y0_3_h = _mm_madd_epi16(pixels0_3_h, RGBScale);
    __m128i y4_7_l = _mm_madd_epi16(pixels4_7_l, RGBScale);
    __m128i y4_7_h = _mm_madd_epi16(pixels4_7_h, RGBScale);*/
    v128_t y0_3_l = wasm_i32x4_dot_i16x8(pixels0_3_l, RGBScale);
    v128_t y0_3_h = wasm_i32x4_dot_i16x8(pixels0_3_h, RGBScale);
    v128_t y4_7_l = wasm_i32x4_dot_i16x8(pixels4_7_l, RGBScale);
    v128_t y4_7_h = wasm_i32x4_dot_i16x8(pixels4_7_h, RGBScale);

    // Horizontal add the result
    /*__m128i y0_3 = _mm_hadd_epi32(y0_3_l, y0_3_h);
    __m128i y4_7 = _mm_hadd_epi32(y4_7_l, y4_7_h);*/
    v128_t y0_3 = hadd_epi32(y0_3_l, y0_3_h);
    v128_t y4_7 = hadd_epi32(y4_7_l, y4_7_h);

    // Shift right by 8 bits to divide by 256
    /*y0_3 = _mm_srli_epi32(y0_3, 8);
    y4_7 = _mm_srli_epi32(y4_7, 8);*/
    y0_3 = wasm_u32x4_shr(y0_3, 8);
    y4_7 = wasm_u32x4_shr(y4_7, 8);

    // Pack the 32-bit results into 16-bit and then into 8-bit values
    /*y0_3 = _mm_packs_epi32(y0_3, y0_3);
    y4_7 = _mm_packs_epi32(y4_7, y4_7);
    y0_3 = _mm_packus_epi16(y0_3, y0_3);
    y4_7 = _mm_packus_epi16(y4_7, y4_7);*/
    y0_3 = wasm_i16x8_narrow_i32x4(y0_3, y0_3);
    y4_7 = wasm_i16x8_narrow_i32x4(y4_7, y4_7);
    y0_3 = wasm_u8x16_narrow_i16x8(y0_3, y0_3);
    y4_7 = wasm_u8x16_narrow_i16x8(y4_7, y4_7);



    // Store the result back to dest
    //*pout++ = _mm_cvtsi128_si32(y0_3);
    //*pout++ = _mm_cvtsi128_si32(y4_7);
    *pout++ = wasm_i32x4_extract_lane(y0_3, 0);
    *pout++ = wasm_i32x4_extract_lane(y4_7, 0);

    numPixelsDiv8--;
  } while (numPixelsDiv8);
}
static void arVideoLumaRGBAtoL_Emscripten_simd128_fast(uint8_t *__restrict dest,
                                                  uint8_t *__restrict src,
                                                  int32_t numPixels) {

  printf("using arVideoLumaRGBAtoL_Emscripten_simd128_fast !!!\n");

  v128_t *pin = (v128_t*)src;
  int64_t* pout = (int64_t*)dest;
  int numPixelsDiv8 = numPixels / 8;

    v128_t maskRedBlue = wasm_i32x4_splat( 0x00FF00FF );
    v128_t scaleRedBlue = wasm_i32x4_splat( (uint32_t)B8_CCIR601 << 16 | R8_CCIR601 );
    v128_t scaleGreen = wasm_i32x4_splat( G8_CCIR601 );
    do
    {
        v128_t pixels1 = wasm_v128_load(pin); // Load 16 bytes (4 pixels) from src
        v128_t pixels2 = wasm_v128_load(pin + 1);
        pin+=2;

        // Shifting uint16 lanes by 8 bits leaves 0 in the higher bytes, no need to mask
        //__m128i g1 = _mm_srli_epi16( pixels1, 8 );
        //__m128i g2 = _mm_srli_epi16( pixels2, 8 );
        v128_t g1 = wasm_u16x8_shr(pixels1, 8);
        v128_t g2 = wasm_u16x8_shr(pixels2, 8);

        // For red and blue channels, bitwise AND with the mask of 0x00FF00FF to isolate
        //__m128i rb1 = _mm_and_si128( pixels1, maskRedBlue );
        //__m128i rb2 = _mm_and_si128( pixels2, maskRedBlue );
        v128_t rb1 = wasm_v128_and( pixels1, maskRedBlue );
        v128_t rb2 = wasm_v128_and( pixels2, maskRedBlue );

        // Scale the numbers, and add pairwise
        /*g1 = _mm_madd_epi16( g1, scaleGreen );
        g2 = _mm_madd_epi16( g2, scaleGreen );
        rb1 = _mm_madd_epi16( rb1, scaleRedBlue );
        rb2 = _mm_madd_epi16( rb2, scaleRedBlue );*/

        g1 = wasm_i32x4_dot_i16x8( g1, scaleGreen );
        g2 = wasm_i32x4_dot_i16x8( g2, scaleGreen );
        rb1 = wasm_i32x4_dot_i16x8( rb1, scaleRedBlue );
        rb2 = wasm_i32x4_dot_i16x8( rb2, scaleRedBlue );

        // We now have them in int32 lanes in the correct order, add vertically
        //v128_t y1 = _mm_add_epi32( g1, rb1 );
        //v128_t y2 = _mm_add_epi32( g2, rb2 );
        v128_t y1 = wasm_i32x4_add( g1, rb1 );
        v128_t y2 = wasm_i32x4_add( g2, rb2 );

        // Divide by 256
        //y1 = _mm_srli_epi32( y1, 8 );
        //y2 = _mm_srli_epi32( y2, 8 );
        y1 = wasm_u32x4_shr( y1, 8 );
        y2 = wasm_u32x4_shr( y2, 8 );

        // Pack 32-bit lanes into unsigned bytes, with saturation
        //__m128i y = _mm_packs_epi32( y1, y2 );
        //y = _mm_packus_epi16( y, y );
        v128_t y = wasm_i16x8_narrow_i32x4( y1, y2 );
        y = wasm_u8x16_narrow_i16x8( y, y );

        // Store 8 bytes with 1 instruction
        //*pout = _mm_cvtsi128_si64( y );
        *pout = wasm_i64x2_extract_lane(y, 0);

        pout++;
        numPixelsDiv8--;
    }
    while( numPixelsDiv8 );
}
static void arVideoLumaRGBAtoL_Intel_simd_asm(uint8_t *__restrict dest,
                                              uint8_t *__restrict src,
                                              int32_t numPixels) {
    printf("using arVideoLumaRGBAtoL_Intel_simd_asm !!!\n");
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

static void arVideoLumaRGBAtoL_Intel_simd_asm_w_mm_add_epi32( uint8_t* __restrict dest,
    uint8_t* __restrict src, int32_t numPixels )
{
    printf("using arVideoLumaRGBAtoL_Intel_simd_asm_w_mm_add_epi32 !!!\n");
    __m128i* pin = ( __m128i* )src;
    int64_t* pout = (int64_t*)dest;
    int numPixelsDiv8 = numPixels / 8;

    __m128i maskRedBlue = _mm_set1_epi32( 0x00FF00FF );
    __m128i scaleRedBlue = _mm_set1_epi32( (uint32_t)B8_CCIR601 << 16 | R8_CCIR601 );
    __m128i scaleGreen = _mm_set1_epi32( G8_CCIR601 );

    do
    {
        __m128i pixels1 = _mm_load_si128( pin );
        __m128i pixels2 = _mm_load_si128( pin + 1 );
        pin += 2;

        // Shifting uint16 lanes by 8 bits leaves 0 in the higher bytes, no need to mask
        __m128i g1 = _mm_srli_epi16( pixels1, 8 );
        __m128i g2 = _mm_srli_epi16( pixels2, 8 );

        // For red and blue channels, bitwise AND with the mask of 0x00FF00FF to isolate
        __m128i rb1 = _mm_and_si128( pixels1, maskRedBlue );
        __m128i rb2 = _mm_and_si128( pixels2, maskRedBlue );

        // Scale the numbers, and add pairwise
        g1 = _mm_madd_epi16( g1, scaleGreen );
        g2 = _mm_madd_epi16( g2, scaleGreen );
        rb1 = _mm_madd_epi16( rb1, scaleRedBlue );
        rb2 = _mm_madd_epi16( rb2, scaleRedBlue );

        // We now have them in int32 lanes in the correct order, add vertically
        __m128i y1 = _mm_add_epi32( g1, rb1 );
        __m128i y2 = _mm_add_epi32( g2, rb2 );

        // Divide by 256
        y1 = _mm_srli_epi32( y1, 8 );
        y2 = _mm_srli_epi32( y2, 8 );

        // Pack 32-bit lanes into unsigned bytes, with saturation
        __m128i y = _mm_packs_epi32( y1, y2 );
        y = _mm_packus_epi16( y, y );

        // Store 8 bytes with 1 instruction
        *pout = _mm_cvtsi128_si64( y );
        pout++;
        numPixelsDiv8--;
    }
    while( numPixelsDiv8 );
}
