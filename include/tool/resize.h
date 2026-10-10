#pragma once
#include "../tImage_definition.h"
#include "../image/Image.h"

namespace tImage {

    // calculate destination size for resize
    DLL_EXPORT t_uintpoint2d calcDstScale(t_uint width, t_uint height, t_floatpoint2d scale);
    // calculate destination size for resize
    DLL_EXPORT t_uintpoint2d calcDstScale(const Image* src, t_floatpoint2d scale);

    // resize image
    DLL_EXPORT t_err resize(const Image* src, Image* dst, t_floatpoint2d scale, _t_floatpoint2d center);

}
