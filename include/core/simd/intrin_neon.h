#pragma once

#include "../../tImage_definition.h"

#include <arm_neon.h>

namespace tImage {
namespace core {
namespace simd {

    struct v_uint8x16 {
        
        uint8x16_t v;

        explicit inline v_uint8x16(void) {} 
        explicit inline v_uint8x16(uint8x16_t v) : v(v) {}

        inline v_uint8x16(
            t_uchar v0, t_uchar v1, t_uchar v2, t_uchar v3,
            t_uchar v4, t_uchar v5, t_uchar v6, t_uchar v7,
            t_uchar v8, t_uchar v9, t_uchar v10, t_uchar v11,
            t_uchar v12, t_uchar v13, t_uchar v14, t_uchar v15
        ) : v{
            v0, v1, v2, v3,
            v4, v5, v6, v7,
            v8, v9, v10, v11,
            v12, v13, v14, v15
        } {}

    };

    /*<******************************************* load *******************************************>*/
    // load aligned 8x16 bit integer to 128 bit register
    inline void v128_load_8x16_aligned(const t_uchar* src, v_uint8x16& dst) {
        v128_load_8x16(src, dst);
    }
    // load uchar 8x16 bit integer to 128 bit register
    inline void v128_load_8x16(const t_uchar* src, v_uint8x16& dst) {
        dst.v = vld1q_u8(src);
    }
    /*\<******************************************* load *******************************************\>*/

    /*<******************************************* store *******************************************>*/
    // store 128 bit register to aligned 8x16 bit integer
    inline void v128_store_8x16_aligned(const v_uint8x16& src, t_uchar* dst) {
        v128_store_8x16(src, dst);
    }
    // store 128 bit register to unaligned 8x16 bit integer
    inline void v128_store_8x16(const v_uint8x16& src, t_uchar* dst) {
        vst1q_u8(dst, src.v);
    }
    /*\<******************************************* store *******************************************\>*/

    /*<******************************************* shuffle *******************************************>*/
    // shuffle 128 bit register by mask
    inline void v128_shuffle_8x16(v_uint8x16& src, v_uint8x16& dst, const v_uint8x16& mask) {
        dst.v = vqtbl1q_u8(src.v, mask.v);
    }
    /*\<******************************************* shuffle *******************************************\>*/

}
}
}