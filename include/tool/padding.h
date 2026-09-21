#pragma once

#include "../tImage_definition.h"
#include "../image/Image.h"

namespace tImage {

    // n以上の最小び2の累乗数を計算
    DLL_EXPORT t_uint calcPaddingSize(t_uint n);

    // 
    DLL_EXPORT t_uintpoint2d calcPaddingSize2d(Image* src);

    // n以上でblockSizeの倍数となるサイズを計算
    DLL_EXPORT t_uint calcPaddingBlockSize(t_uint n, t_uint blockSize);

}
