#pragma once
#include "../../tImage_definition.h"
#include "../simd/simd.h"

namespace tImage {
namespace core {
namespace ip {

    template <typename T>
    inline T nearest(const T* src, t_floatpoint2d point) {

        const t_int x = static_cast<t_int>(std::round(point.x));
        const t_int y = static_cast<t_int>(std::round(point.y));

        return src[y * 1 + x];

    }

}
}
}