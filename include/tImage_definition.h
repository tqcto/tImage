#pragma once
#ifndef _T_IMAGE_DEFINITION_H_
#define _T_IMAGE_DEFINITION_H_

#include <cstdint>

namespace tImage {

// MS
#if defined(_MSC_VER)
	#define T_MS		1
#endif

// MINGW32
#if defined(__MINGW32__)
	#define T_MINGW32	1
#endif

/*
// AVX2
#if defined(__AVX2__)
	#define T_AVX2		1
#endif

// AVX
#if defined(__AVX__)
	#define T_AVX		1
#endif

// SSE2
#if defined(__SSE2__) || defined(_M_AMD64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
	#define T_SSE2		1
#endif

// ARM NEON
#if defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(_M_ARM64)
	#define T_NEON		1
#endif

// Wasm SIMD
#if defined(__wasm_simd128__)
	#define T_WASM_SIMD	1
#endif
*/

#ifndef DLL_EXPORT
#if defined(_WIN32) && defined(TIMAGE_BUILD_DLL)
#define DLL_EXPORT __declspec(dllexport)
#elif defined(_WIN32) && defined(TIMAGE_USE_DLL)
#define DLL_EXPORT __declspec(dllimport)
#else
#define DLL_EXPORT
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

typedef struct _t_intpoint2d {

	t_int x;
	t_int y;

	explicit _t_intpoint2d(t_int x, t_int y) : x{x}, y{y} {}

}t_point2d;

typedef struct _t_uintpoint2d {

	t_uint x;
	t_uint y;

	explicit _t_uintpoint2d(t_uint x, t_uint y) : x{x}, y{y} {}

}t_uintpoint2d;

#define T_IMAGE_DEFAULT_ALIGN	32
/*
#if defined(_MSC_VER) || defined(__MINGW32__)
#if defined(__AVX2__)
#include <immintrin.h>
// alignment size
#define T_IMAGE_DEFAULT_ALIGN	alignof(__m256i)//32
#endif
#else
#define T_IMAGE_DEFAULT_ALIGN	alignof(float32x4_t)
#endif
*/

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