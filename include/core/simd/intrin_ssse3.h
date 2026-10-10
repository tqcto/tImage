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
            t_uchar v0, t_uchar v1, t_uchar v2, t_uchar v3,
            t_uchar v4, t_uchar v5, t_uchar v6, t_uchar v7,
            t_uchar v8, t_uchar v9, t_uchar v10, t_uchar v11,
            t_uchar v12, t_uchar v13, t_uchar v14, t_uchar v15
        ) : v(_mm_set_epi8(
                v15, v14, v13, v12, v11, v10, v9,  v8,
                v7,  v6,  v5,  v4,  v3,  v2,  v1,  v0
            )) {}

    };

    struct v_float32x4 {
        
        __m128 v;

        explicit inline v_float32x4(void) {} 
        explicit inline v_float32x4(__m128 v) : v(v) {}

        inline v_float32x4(
            t_float v0, t_float v1, t_float v2, t_float v3
        ) : v(_mm_set_ps(v3, v2, v1, v0)) {}

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

    // load aligned 32x4 bit float to 128 bit register
    inline void v128_load_32x4_aligned(t_float* src, v_float32x4& dst) {
        dst.v = _mm_load_ps(src);
    }
    // load unaligned 32x4 bit float to 128 bit register
    inline void v128_load_32x4(t_float* src, v_float32x4& dst) {
        dst.v = _mm_loadu_ps(src);
    }
    /*\<******************************************* load *******************************************\>*/

    /*<******************************************* store *******************************************>*/
    // store 128 bit register to aligned 8x16 bit integer
    inline void v128_store_8x16_aligned(v_uint8x16& src, t_uchar* dst) {
        _mm_store_si128(reinterpret_cast<__m128i*>(dst), src.v);
    }
    // store 128 bit register to unaligned 8x16 bit integer
    inline void v128_store_8x16(v_uint8x16& src, t_uchar* dst) {
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), src.v);
    }

    // store 128 bit register to aligned 32x4 bit float
    inline void v128_store_32x4_aligned(v_float32x4& src, t_float* dst) {
        _mm_store_ps(dst, src.v);
    }
    // store 128 bit register to unaligned 32x4 bit float
    inline void v128_store_32x4(v_float32x4& src, t_float* dst) {
        _mm_storeu_ps(dst, src.v);
    }
    /*\<******************************************* store *******************************************\>*/

    /*<******************************************* shuffle *******************************************>*/
    // shuffle 128 bit register by mask
    inline void v128_shuffle_8x16(v_uint8x16& src, v_uint8x16& dst, const v_uint8x16& mask) {
        dst.v = _mm_shuffle_epi8(src.v, mask.v);
    }

    // shuffle 128 bit register by mask
    // inline void v128_shuffle_32x4(v_float32x4& src, v_float32x4& dst, const v_float32x4& mask) {
    //     dst.v = _mm_shuffle_ps(src.v, mask.v);
    // }
    /*\<******************************************* shuffle *******************************************\>*/

    /*<******************************************* round *******************************************>*/
    // round 128 bit float register
    inline void v128_round_float32x4(v_float32x4& src, v_float32x4& dst) {
        dst.v = _mm_round_ps(src.v, (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC));
    }

    // round 128 bit double register
    // inline void v128_round_float64x2(v_float64x2& src, v_float64x2& dst) {
    //     dst.v = _mm_round_pd(src.v, (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC));
    // }
    /*\<******************************************* round *******************************************\>*/

}
}
}