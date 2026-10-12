#pragma once
#include "../tImage_definition.h"
#include "../core/codec/codec.h"

namespace tImage {

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

	/*<******************************************* friend functions *******************************************>*/
	// swap channels
    // ex: RGB <-> BGR
	LIB_EXPORT t_err swap(Image* src, Image* dst) noexcept;
	/*/<******************************************* friend functions *******************************************>/*/

	class Image {

	private:

		t_uint _align = T_IMAGE_DEFAULT_ALIGN;	// 32byte align

		t_uint _width = 0;
		t_uint _height = 0;

		t_uint64 _elements_row = 0;   // elements of a row
		t_uint64 _stride = 0;	    // bytes of a row
		
		//t_uint _channels = 8;
		//t_uint _depth = 8;		// Color depth. usually 8.
		PixelFormat format;		// pixel format
		//t_colorType _colorType = t_colorType_RGB;

		// Is memory from external
		t_bool _external_memory = false;

		t_err _allocate_memory();

		/*
		void bilinear() {



		}
		*/

	public:

		// image data
		t_uchar* data = nullptr;

		// initialize class

		/* Initialize empty class */
		LIB_EXPORT Image(void);
		/* Initialize class from allocate function */
		LIB_EXPORT Image(t_uint width, t_uint height, t_uint channels, core::codec::t_colorType colorType);
		/* Initialize class from allocate function */
		LIB_EXPORT Image(t_uint width, t_uint height, t_uint channels, core::codec::t_colorType colorType, t_uint depth);
		//LIB_EXPORT Image(t_uint width, t_uint height, t_uint channles, BitsPerChannel unpacked_format);

		LIB_EXPORT ~Image();

		/* Set align. Only powers of 2 can be specified. */
		LIB_EXPORT t_err setAlign(t_uint align) noexcept;

		/* Input image of other memory*/
		LIB_EXPORT t_err input(
			t_uchar* src,
			t_uint width, t_uint height,
			t_uint channels, core::codec::t_colorType colorType, t_uint depth,
			t_uint align
		);

		t_err allocate(t_uint cols, t_uint rows) = delete;

		/* Allocate memory of image */
		LIB_EXPORT t_err allocate(t_uint width, t_uint height, t_uint channels, core::codec::t_colorType colorType);
		/* Allocate memory of image. depth is byte count. */
		LIB_EXPORT t_err allocate(t_uint width, t_uint height, t_uint channels, core::codec::t_colorType colorType, t_uint depth);

		/* Release memory */
		LIB_EXPORT void release();

		/* Get whether class is empty. If empty then returned true. */
		LIB_EXPORT t_bool empty() const noexcept;

		// 書き込み用行ポインタを取得
        inline t_uchar* rowPtr(t_uint row) const noexcept {

            return &this->data[row * this->_elements_row];

        }

		/* Get width of image */
		inline t_uint width() const noexcept {
			return this->_width;
		}
		/* Get height of image */
		inline t_uint height() const noexcept {
			return this->_height;
		}
		// 1行の要素数を取得
        inline t_uint64 elementsRow() const noexcept {
            return this->_elements_row;
        }
		/* Get stride of image */
		inline t_uint64 stride() const noexcept {
            return this->_stride;
        }
        inline t_uint align() const noexcept {
            return this->_align;
        }
		/* Get stride of image */
		// LIB_EXPORT t_uint64 stride() const noexcept;
		/* Get channels of image */
		inline t_uint channels() const noexcept {
			return this->format.channels;
		}
		/* Get color type of image */
		inline core::codec::t_colorType colorType() const noexcept {
			return this->format.color_type;
		}
		/* Get bit depth of image */
		LIB_EXPORT t_uint depth() const noexcept;
		/* Get byte depth of image */
		LIB_EXPORT t_uint depthByte() const noexcept;

		/* convert color channel */
		//LIB_EXPORT t_err convertColorType(t_colorType type);

		//LIB_EXPORT t_err fill();

		LIB_EXPORT t_bool operator==(const Image& img) const;
		
		LIB_EXPORT t_uchar& operator()(t_uint x, t_uint y);
		//LIB_EXPORT t_uchar& operator()(t_uint x, t_uint y, t_uint c);

		friend t_err swap(Image* src, Image* dst) noexcept;

	};

	LIB_EXPORT t_err decodePNG(Image* dst, const char* filepath);
	LIB_EXPORT t_err encodePNG(Image* src, const char* filepath);
	LIB_EXPORT t_err decodeJPEG(Image* dst, const char* filepath);
	LIB_EXPORT t_err encodeJPEG(Image* src, const char* filepath);

}