#include "../../include/tool/padding.h"

namespace tImage {
    
    // n以下の最大の2の累乗数を計算
    inline t_uint bit_floor(t_uint n) {

        // t_uint is 32bit
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;

        return n - (n >> 1);

    }

    t_uint calcPaddingSize(t_uint n) {

        //t_uint ceilN = std::bit_ceil(n); // C++20
        t_uint floorN = bit_floor(n);
        return floorN << (n != floorN);

    }

    t_uintpoint2d calcPaddingSize2d(Image* src) {

        return t_uintpoint2d(
            calcPaddingSize(src->width()),
            calcPaddingSize(src->height())
        );

    }

    t_uint calcPaddingBlockSize(t_uint n, t_uint blockSize) {

        if (blockSize == 0) return 0;
        return ((n + blockSize - 1) / blockSize) * blockSize;

    }

}
