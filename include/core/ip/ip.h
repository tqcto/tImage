#include "../../tImage_definition.h"

#include "nearest.h"
#include "bilinear.h"
#include "bicubic.h"
#include "lanczos.h"

namespace tImage {

    enum t_ip : t_uint {

		t_ip_Nearest	= 0L,
		t_ip_Bilinear	= 1L << 0L,
		t_ip_Bicubic	= 1L << 1L,
		t_ip_Lanczos	= 1L << 2L,

	};

}