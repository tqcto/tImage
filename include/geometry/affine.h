#pragma once
#include "../tImage_definition.h"
#include "../image/Image.h"

namespace tImage {

    DLL_EXPORT void resize(const Image* src, Image* dst, t_float scale_x, t_float scale_y);

}
