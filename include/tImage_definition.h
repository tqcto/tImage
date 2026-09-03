#pragma once
#ifndef _T_IMAGE_DEFINITION_H_
#define _T_IMAGE_DEFINITION_H_

#include <cstdint>

namespace tImage {

#ifndef DLL_EXPORT
#ifdef _DLL
#define DLL_EXPORT	__declspec(dllexport)
#else
#define DLL_EXPORT	__declspec(dllimport)
#endif
#endif

#ifdef __cplusplus
    typedef bool                t_bool;
#else
    typedef char                t_bool;
#endif

typedef int8_t              t_char;     // 確実に1byte
typedef int16_t             t_short;    // 確実に2byte
typedef int32_t             t_int;      // 確実に4byte
typedef float               t_float;    // 4byte
typedef double              t_double;   // 8byte

typedef int64_t             t_int64;    // MSVCでもGCCでも確実に8byte

typedef uint32_t            t_uint;     // 符号なし4byte
typedef uint8_t             t_uchar;    // 符号なし1byte (画素データに最適)
typedef uint64_t            t_uint64;   // 符号なし8byte

// for SIMD�i32byte�j
#define T_IMAGE_DEFAULT_ALIGN	32

	typedef t_uint	t_flags;
	typedef t_int	t_err;

	enum {

		t_err_None						= 0L,
		t_err_InvalidArgument			= 1L << 0L,
		t_err_MemoryAllocationFailed	= 1L << 1L,
		t_err_MemoryAccessFailed		= 1L << 2L,
		t_err_CanNotOpenedFile			= 1L << 3L,
		t_err_CanNotWrittenFile			= 1L << 4L,

	};

}

#endif