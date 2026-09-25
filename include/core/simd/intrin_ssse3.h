#pragma once

#include "../../tImage_definition.h"

#include <tmmintrin.h>

namespace tImage {
namespace core {
namespace simd {

    struct v_uint8x16 {
        
        __m128i v;

        explicit inline v_uint8x16(void) {} 
        explicit inline v_uint8x16(__m128i v) : v(v) {}

        inline v_uint8x16(
            t_char v0, t_char v1, t_char v2, t_char v3,
            t_char v4, t_char v5, t_char v6, t_char v7,
            t_char v8, t_char v9, t_char v10, t_char v11,
            t_char v12, t_char v13, t_char v14, t_char v15
        ) : v(_mm_set_epi8(
                v15, v14, v13, v12, v11, v10, v9,  v8,
                v7,  v6,  v5,  v4,  v3,  v2,  v1,  v0
            )) {}

    };

    /*<******************************************* load *******************************************>*/
    // load aligned 8x16 bit integer to 128 bit register
    inline void v128_load_8x16_aligned(t_uchar* src, v_uint8x16& dst) {
        dst.v = _mm_load_si128(reinterpret_cast<const __m128i*>(src));
    }
    // load uchar 8x16 bit integer to 128 bit register
    inline void v128_load_8x16(t_uchar* src, v_uint8x16& dst) {
        dst.v = _mm_lddqu_si128 (reinterpret_cast<const __m128i*>(src));
    }
    /*\<******************************************* load *******************************************\>*/

    /*<******************************************* store *******************************************>*/
    // store 128 bit register to aligned 8x16 bit integer
    inline void v128_store_8x16_aligned(v_uint8x16& src, t_uchar* dst) {
        _mm_store_si128(reinterpret_cast<__m128i*>(dst), src.v);
    }
    // store 128 bit register to unaligned 8x16 bit integer
    inline void v256_store_8x32(v_uint8x16& src, t_uchar* dst) {
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), src.v);
    }
    /*\<******************************************* store *******************************************\>*/

    /*<******************************************* shuffle *******************************************>*/
    // shuffle 128 bit register by mask
    inline void v128_shuffle_8x16(v_uint8x16& src, v_uint8x16& dst, v_uint8x16& mask) {
        dst.v = _mm_shuffle_epi8(src.v, mask.v);
    }
    /*\<******************************************* shuffle *******************************************\>*/

}
}
}