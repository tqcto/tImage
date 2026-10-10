#pragma once
#include "../../tImage_definition.h"
#include "../cpu.h"
#include "../simd/simd.h"

using namespace tImage::core::simd;

namespace tImage {
namespace core {
namespace ip {

    template <typename T>
    inline T nearest(const T* src, t_floatpoint2d point) {

        const t_int x = static_cast<t_int>(std::round(point.x));
        const t_int y = static_cast<t_int>(std::round(point.y));

        return src[y * 1 + x];

    }

    // inline v_uint8x32 nearest(const v_uint8x32 src, v_float32x8 pointsX, v_float32x8 pointsY) {

    //     v_float32x8 dstX, dstY;
    //     v256_round_float32x8(pointsX, dstX);
    //     v256_round_float32x8(pointsY, dstY);



    // }

}
}
}