#pragma once

#include "../../tImage_definition.h"

#include <stdio.h>

#define SIGNATURE_NUM	8

namespace tImage {
namespace core {
namespace codec {

	typedef enum {

		t_colorType_Unknown			= 0,

		/* gray scale */
		t_colorType_GrayScale		= 1L << 0L,
		t_colorType_GrayScaleAlpha	= 1L << 1L,
		
		/* 3 channels */
		
		t_colorType_RGB				= 1L << 2L,
		t_colorType_BGR				= 1L << 3L,
		t_colorType_HSV				= 1L << 4L,
		t_colorType_YUV				= 1L << 5L,
		t_colorType_YCbCr			= 1L << 6L,

		/* 3 channels +alpha channel */

		t_colorType_RGBA			= 1L << 7L,
		t_colorType_BGRA			= 1L << 8L,

	}t_colorType;

    typedef struct {
		t_uint width;
		t_uint height;
		t_uint64 stride;
		t_uint channels;
		t_colorType colorType;
		t_uint depth;
	}t_ImageFile_Header;

    inline t_err t_fopen(FILE** fpP, const char* filepath, const char* mode) {

#ifdef _WIN32

		return !fopen_s(fpP, filepath, mode) ? t_err_None : t_err_CanNotOpenedFile;


#else

		*fpP = fopen(filepath, mode);
		return *fpP != nullptr ? t_err_None : t_err_CanNotOpenedFile;

#endif

	}

}
}
}