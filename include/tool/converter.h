#pragma once

#include "..\tImage_definition.h"
#include "..\image\Image.h"

#include <immintrin.h>

namespace tImage {

	DLL_EXPORT t_err RGB_BGR(Image* src, Image* dst);

	DLL_EXPORT t_err split(Image* src, Image* dst_planes);
	DLL_EXPORT t_err merge(Image* src_planes, Image* dst);

}