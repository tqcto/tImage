#pragma once

#include "../../tImage_definition.h"

#include <immintrin.h>

namespace tImage {
namespace core {
namespace simd {

    struct v_uint8x32 {
        
        __m256i v;

        explicit inline v_uint8x32(void) {} 
        explicit inline v_uint8x32(__m256i v) : v(v) {}

        inline v_uint8x32(
            t_uchar v0, t_uchar v1, t_uchar v2, t_uchar v3,
            t_uchar v4, t_uchar v5, t_uchar v6, t_uchar v7,
            t_uchar v8, t_uchar v9, t_uchar v10, t_uchar v11,
            t_uchar v12, t_uchar v13, t_uchar v14, t_uchar v15,
            t_uchar v16, t_uchar v17, t_uchar v18, t_uchar v19,
            t_uchar v20, t_uchar v21, t_uchar v22, t_uchar v23,
            t_uchar v24, t_uchar v25, t_uchar v26, t_uchar v27,
            t_uchar v28, t_uchar v29, t_uchar v30, t_uchar v31
        ) : v(_mm256_set_epi8(
                v31, v30, v29, v28, v27, v26, v25, v24,
                v23, v22, v21, v20, v19, v18, v17, v16,
                v15, v14, v13, v12, v11, v10, v9,  v8,
                v7,  v6,  v5,  v4,  v3,  v2,  v1,  v0
            )) {}

    };

    /*<******************************************* load *******************************************>*/
    // load aligned 8x32 bit integer to 256 bit register
    inline void v256_load_8x32_aligned(t_uchar* src, v_uint8x32& dst) {
        dst.v = _mm256_load_si256(reinterpret_cast<const __m256i*>(src));
    }
    // load uchar 8x32 bit integer to 256 bit register
    inline void v256_load_8x32(t_uchar* src, v_uint8x32& dst) {
        dst.v = _mm256_lddqu_si256 (reinterpret_cast<const __m256i*>(src));
    }
    /*\<******************************************* load *******************************************\>*/

    /*<******************************************* store *******************************************>*/
    // store 256 bit register to aligned 8x32 bit integer
    inline void v256_store_8x32_aligned(v_uint8x32& src, t_uchar* dst) {
        _mm256_store_si256(reinterpret_cast<__m256i*>(dst), src.v);
    }
    // store 256 bit register to unaligned 8x32 bit integer
    inline void v256_store_8x32(v_uint8x32& src, t_uchar* dst) {
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst), src.v);
    }
    /*\<******************************************* store *******************************************\>*/

    /*<******************************************* shuffle *******************************************>*/
    // shuffle 256 bit register by mask
    inline void v256_shuffle_8x32(v_uint8x32& src, v_uint8x32& dst, const v_uint8x32& mask) {
        dst.v = _mm256_shuffle_epi8(src.v, mask.v);
    }
    /*\<******************************************* shuffle *******************************************\>*/

}
}
}