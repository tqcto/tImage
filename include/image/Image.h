#pragma once
#include "../tImage_definition.h"

namespace tImage {

	enum {

		t_transformFlag_NONE		= 0L,
		t_transformFlag_TILE		= 1L,
		t_transformFlag_MIRROR		= 1L << 1L,

		t_transformFlag_MOVE		= 1L << 2L,
		t_transformFlag_ROTATION	= 1L << 3L,
		t_transformFlag_SCALING		= 1L << 4L,

	};

	typedef enum {

		/* gray scale */
		t_colorType_GrayScale		= 1L << 0L,
		
		/* 3 channels */
		
		t_colorType_RGB				= 1L << 1L,
		t_colorType_BGR				= 1L << 2L,
		t_colorType_YUV				= 1L << 3L,
		t_colorType_YCbBr			= 1L << 4L,

		/* +alpha channel */

		t_colorType_RGBA			= 1L << 5L,
		t_colorType_BGRA			= 1L << 6L,

	}t_colorType;

	/*
	// Bits per channel.
	typedef struct {
		t_uint r;
		t_uint g;
		t_uint b;
		t_uint a;
	}BitsPerChannel;
	*/

	// Pixel format for Image class.
	typedef struct {
		t_uint channels;
		/*
		BitsPerChannel bits_per_channel;
		t_bool packed;
		*/
		t_uint bytes_per_pixel;		// if packed is true, then use this.
	}PixelFormat;

	class Image {

	private:

		t_uint _align = T_IMAGE_DEFAULT_ALIGN;	// 32byte align

		t_uint _width = 0;
		t_uint _height = 0;
		t_uint64 _stride = 0;		// length of a row
		//t_uint _channels = 8;
		//t_uint _depth = 8;		// Color depth. usually 8.
		PixelFormat format;		// pixel format
		//t_colorType _colorType = t_colorType_RGB;

		// Is memory from external
		t_bool _external_memory = false;

		t_err _allocate_memory();

		void bilinear() {



		}

	public:

		// image data
		t_uchar* data = nullptr;

		// initialize class

		/* Initialize empty class */
		DLL_EXPORT Image(void);
		/* Initialize class from allocate function */
		DLL_EXPORT Image(t_uint width, t_uint height, t_uint channels);
		/* Initialize class from allocate function */
		DLL_EXPORT Image(t_uint width, t_uint height, t_uint channels, t_uint depth);
		//DLL_EXPORT Image(t_uint width, t_uint height, t_uint channles, BitsPerChannel unpacked_format);

		DLL_EXPORT ~Image();

		/* Set align. Only powers of 2 can be specified. */
		DLL_EXPORT t_err setAlign(t_uint align);

		/* Input image of other memory*/
		DLL_EXPORT t_err input(
			t_uchar* src,
			t_uint width, t_uint height,
			t_uint channels, t_uint depth,
			t_uint align
		);

		/* Allocate memory of image */
		DLL_EXPORT t_err allocate(t_uint width, t_uint height, t_uint channels);
		/* Allocate memory of image. depth is byte count. */
		DLL_EXPORT t_err allocate(t_uint width, t_uint height, t_uint channels, t_uint depth);

		/* Release memory */
		DLL_EXPORT void release();

		/* Get whether class is empty. If empty then returned true. */
		DLL_EXPORT t_bool empty() const noexcept;

		/* Get width of image */
		DLL_EXPORT t_uint width() const noexcept;
		/* Get height of image */
		DLL_EXPORT t_uint height() const noexcept;
		/* Get stride of image */
		DLL_EXPORT t_uint64 stride() const noexcept;
		/* Get channels of image */
		DLL_EXPORT t_uint channels() const noexcept;
		/* Get bit depth of image */
		DLL_EXPORT t_uint depth() const noexcept;
		/* Get byte depth of image */
		DLL_EXPORT t_uint depthByte() const noexcept;
		/* Get color type of image */
		//DLL_EXPORT t_colorType colorType() const noexcept;

		DLL_EXPORT t_err RGB2BGR(void);
		DLL_EXPORT t_err RGBA2BGRA(void);
		/* convert color channel */
		//DLL_EXPORT t_err convertColorType(t_colorType type);

		//DLL_EXPORT t_err fill();

		DLL_EXPORT t_bool operator==(const Image& img) const;

	};

	DLL_EXPORT t_err decodePNG(Image* dst, const char* filepath);
	DLL_EXPORT t_err encodePNG(Image* src, const char* filepath);

}