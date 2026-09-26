#pragma once

#include "../../tImage_definition.h"
#include "codec.h"

namespace tImage {
namespace core {
namespace codec {

    DLL_EXPORT t_err readPNG(t_ImageFile_Header* png_header, const char* filepath);
	DLL_EXPORT t_err decodePNG(t_ImageFile_Header* in_data, t_uchar* dst, const char* filepath);
	DLL_EXPORT t_err writePNG(t_ImageFile_Header* in_data, t_uchar* src, const char* filepath);    

}
}
}