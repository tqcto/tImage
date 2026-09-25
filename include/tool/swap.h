#pragma once
#include "../../include/image/Image.h"
#include "../../include/tImage_definition.h"

namespace tImage {

    // swap channels
    // ex: RGB <-> BGR
    DLL_EXPORT t_err swap(Image* src, Image* dst);

}