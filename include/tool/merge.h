#pragma once

#include "../tImage_definition.h"
#include "../image/Image.h"

namespace tImage {

	LIB_EXPORT t_err merge(Image* src_planes, Image* dst);

}