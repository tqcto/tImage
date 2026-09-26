#pragma once

#include "../../tImage_definition.h"
#include "codec.h"

namespace tImage {
namespace core {
namespace codec {

    DLL_EXPORT t_err readJPEG(t_ImageFile_Header* jpeg_header, const char* filepath);
	DLL_EXPORT t_err decodeJPEG(t_ImageFile_Header* in_data, t_uchar* dst, const char* filepath);
	DLL_EXPORT t_err writeJPEG(t_ImageFile_Header* in_data, t_uchar* src, const char* filepath);
    
}
}
}