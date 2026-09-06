#pragma once

#include "..\tImage_definition.h"
#include "..\image\Image.h"

namespace tImage {

	DLL_EXPORT t_err fft(Image* src, Image* dst_real, Image* dst_imag);

}