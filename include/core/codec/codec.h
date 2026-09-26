#pragma once

#include "../../tImage_definition.h"

#include <stdio.h>

#define SIGNATURE_NUM	8

namespace tImage {
namespace core {
namespace codec {

    typedef struct {
		t_uint width;
		t_uint height;
		t_uint64 stride;
		t_uint channels;
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