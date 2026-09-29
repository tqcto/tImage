#pragma once
#include "../tImage_definition.h"
#include "Matrix.h"
#include "core/codec/codec.h"

namespace tImage {

	enum {

		t_transformFlag_NONE		= 0L,
		t_transformFlag_TILE		= 1L,
		t_transformFlag_MIRROR		= 1L << 1L,

		t_transformFlag_MOVE		= 1L << 2L,
		t_transformFlag_ROTATION	= 1L << 3L,
		t_transformFlag_SCALING		= 1L << 4L,

	};

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
		core::codec::t_colorType color_type;
		/*
		BitsPerChannel bits_per_channel;
		t_bool packed;
		*/
		t_uint bytes_per_pixel;		// if packed is true, then use this.
	}PixelFormat;

	class Image;
	DLL_EXPORT t_err swap(Image* src, Image* dst) noexcept;

	class Image : public Matrix<t_uchar> {

	private:

		// cols: t_uint _width = 0;
		// rows: t_uint _height = 0;
		
		//t_uint _channels = 8;
		//t_uint _depth = 8;		// Color depth. usually 8.
		PixelFormat format;		// pixel format
		//t_colorType _colorType = t_colorType_RGB;

		t_err _allocate_memory();

		/*
		void bilinear() {



		}
		*/

	public:

		// image data
		// t_uchar* data = nullptr;

		// initialize class

		/* Initialize empty class */
		DLL_EXPORT Image(void);
		/* Delete */
		Image(t_uint cols, t_uint rows) = delete;
		/* Initialize class from allocate function */
		DLL_EXPORT Image(t_uint width, t_uint height, t_uint channels, core::codec::t_colorType colorType);
		/* Initialize class from allocate function */
		DLL_EXPORT Image(t_uint width, t_uint height, t_uint channels, core::codec::t_colorType colorType, t_uint depth);
		//DLL_EXPORT Image(t_uint width, t_uint height, t_uint channles, BitsPerChannel unpacked_format);

		DLL_EXPORT ~Image();

		/* Set align. Only powers of 2 can be specified. */
		// DLL_EXPORT t_err setAlign(t_uint align);

		/* Input image of other memory*/
		DLL_EXPORT t_err input(
			t_uchar* src,
			t_uint width, t_uint height,
			t_uint channels, core::codec::t_colorType colorType, t_uint depth,
			t_uint align
		);

		t_err allocate(t_uint cols, t_uint rows) = delete;

		/* Allocate memory of image */
		DLL_EXPORT t_err allocate(t_uint width, t_uint height, t_uint channels, core::codec::t_colorType colorType);
		/* Allocate memory of image. depth is byte count. */
		DLL_EXPORT t_err allocate(t_uint width, t_uint height, t_uint channels, core::codec::t_colorType colorType, t_uint depth);

		/* Release memory */
		DLL_EXPORT void release();

		/* Get whether class is empty. If empty then returned true. */
		DLL_EXPORT t_bool empty() const noexcept;

		/* Delete */
		t_uint cols() const noexcept = delete;
		t_uint rows() const noexcept = delete;

		/* Get width of image */
		inline t_uint width() const noexcept {
			return this->_cols;
		}
		/* Get height of image */
		inline t_uint height() const noexcept {
			return this->_rows;
		}
		/* Get stride of image */
		// DLL_EXPORT t_uint64 stride() const noexcept;
		/* Get channels of image */
		inline t_uint channels() const noexcept {
			return this->format.channels;
		}
		/* Get color type of image */
		inline core::codec::t_colorType colorType() const noexcept {
			return this->format.color_type;
		}
		/* Get bit depth of image */
		DLL_EXPORT t_uint depth() const noexcept;
		/* Get byte depth of image */
		DLL_EXPORT t_uint depthByte() const noexcept;

		/* convert color channel */
		//DLL_EXPORT t_err convertColorType(t_colorType type);

		//DLL_EXPORT t_err fill();

		DLL_EXPORT t_bool operator==(const Image& img) const;
		
		DLL_EXPORT t_uchar& operator()(t_uint x, t_uint y);
		//DLL_EXPORT t_uchar& operator()(t_uint x, t_uint y, t_uint c);

		friend t_err swap(Image* src, Image* dst) noexcept;

	};

	DLL_EXPORT t_err decodePNG(Image* dst, const char* filepath);
	DLL_EXPORT t_err encodePNG(Image* src, const char* filepath);
	DLL_EXPORT t_err decodeJPEG(Image* dst, const char* filepath);
	DLL_EXPORT t_err encodeJPEG(Image* src, const char* filepath);

}