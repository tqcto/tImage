#pragma once
#include "../tImage_definition.h"
#include "../image/Image.h"

namespace tImage {

    DLL_EXPORT t_err resize(const Image* src, Image* dst, t_floatpoint2d scale, _t_floatpoint2d center);

}
