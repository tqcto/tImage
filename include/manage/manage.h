#pragma once

#include "../tImage_definition.h"

#include <stdio.h>

namespace tImage {

	DLL_EXPORT t_uint64 calcStride4Matrix(t_uint cols, t_uint64 size, t_uint align);
	DLL_EXPORT t_uint64 calcStride(t_uint width, t_uint channels, t_uint depth, t_uint align);

}