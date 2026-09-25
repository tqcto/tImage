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
            t_char v0, t_char v1, t_char v2, t_char v3,
            t_char v4, t_char v5, t_char v6, t_char v7,
            t_char v8, t_char v9, t_char v10, t_char v11,
            t_char v12, t_char v13, t_char v14, t_char v15,
            t_char v16, t_char v17, t_char v18, t_char v19,
            t_char v20, t_char v21, t_char v22, t_char v23,
            t_char v24, t_char v25, t_char v26, t_char v27,
            t_char v28, t_char v29, t_char v30, t_char v31
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
    inline void v256_shuffle_8x32(v_uint8x32& src, v_uint8x32& dst, v_uint8x32& mask) {
        dst.v = _mm256_shuffle_epi8(src.v, mask.v);
    }
    /*\<******************************************* shuffle *******************************************\>*/

}
}
}