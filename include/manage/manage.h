#pragma once

#include "..\tImage_definition.h"

#include <stdio.h>

namespace tImage {

	typedef struct {
		t_uint width;
		t_uint height;
		t_uint stride;
		t_uint channels;
		t_uint depth;
	}t_ImageFile_Header;

	DLL_EXPORT t_uint64 calcStride(t_uint width, t_uint channels, t_uint depth, t_uint align);

	/* PNG */
	
	DLL_EXPORT t_err readPNG(t_ImageFile_Header* png_header, const char* filepath);
	DLL_EXPORT t_err decodePNG(t_ImageFile_Header* in_data, t_uchar* dst, const char* filepath);
	DLL_EXPORT t_err writePNG(t_ImageFile_Header* in_data, t_uchar* src, const char* filepath);

	/* PNG */

}