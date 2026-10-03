#include "../../include/image/Image.h"

#include "../../include/core/stride.h"
#include "../../include/core/codec/codec.h"
#include "../../include/core/codec/png.h"
#include "../../include/core/codec/jpeg.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h> // for assert
#if defined(__AVX2__)
#include <immintrin.h>
#endif

namespace tImage {

	inline t_err Image::_allocate_memory() {

		// calc stride
		this->_stride = core::calcStride(this->_cols, this->format.channels, this->depth(), this->_align);

		// calc elements of a row
		this->_elements_row = this->_stride / sizeof(t_uchar);

		// total bytes
		t_uint64 total_bytes = this->_stride * (t_uint64)this->_rows;

		this->data = core::mem::alignedMalloc<t_uchar>(total_bytes, this->_align);
		// this->data = (t_uchar*)malloc(total_bytes);

		return this->data != nullptr ? t_err_None : t_err_MemoryAllocationFailed;

	}

	Image::Image(void) {
	
		

	}
	Image::Image(t_uint width, t_uint height, t_uint channels, core::codec::t_colorType colorType) {

		t_err err = this->allocate(width, height, channels, colorType);

		switch (err) {

		case t_err_InvalidArgument:
			throw "invalid argument.";
			break;

		case t_err_MemoryAllocationFailed:
			throw "memory allocation failed.";
			break;

		default:
			break;

		}

	}
	Image::Image(t_uint width, t_uint height, t_uint channels, core::codec::t_colorType colorType, t_uint depth) {

		t_err err = this->allocate(width, height, channels, colorType, depth);

		switch (err) {

		case t_err_InvalidArgument:
			throw "invalid argument.";
			break;

		case t_err_MemoryAllocationFailed:
			throw "memory allocation failed.";
			break;

		default:
			break;

		}

	}
	/*
	Image::Image(t_uint width, t_uint height, t_uint channels, BitsPerChannel unpacked_format) {

		if (!width || !height || !channels) throw "invalid argument.";

		this->_width = width;
		this->_height = height;

		this->format.channels = channels;
		this->format.packed = false;
		this->format.bits_per_channel = unpacked_format;

		t_err err = this->_allocate_memory();

		if (err != t_err_None) throw "memory allocation failed.";

	}
	*/

	Image::~Image() {

		if (this->data) {
			this->release();
		}

	}

	/*
	t_err Image::setAlign(t_uint align) {

		// 2�ׂ̂��悩����
		if (align & (align - 1) && !(this->data)) return t_err_InvalidArgument;
		
		this->_align = align;
		return t_err_None;

	}
	*/

	t_err Image::input(
		t_uchar* src,
		t_uint width, t_uint height,
		t_uint channels, core::codec::t_colorType colorType, t_uint depth,
		t_uint align
	) {

		t_flags err = t_err_None;

		err = this->setAlign(align);
		if (err != t_err_None) return err;

		err = !width || !height || !channels ? t_err_InvalidArgument : t_err_None;

		this->_cols = width;
		this->_rows = height;

		this->format.channels = channels;
		this->format.color_type = colorType;
		this->format.bytes_per_pixel = (depth >> 3) * channels;	// usually 8bit (=1byte)
		
		err |= this->setAlign(align);
		if (err != t_err_None) return err;

		this->_stride = core::calcStride(width, channels, depth, _align);
		this->_elements_row = this->_stride / sizeof(t_uchar);

		this->data = src;
		this->_external_memory = true;

		return err;

	}

	t_err Image::allocate(t_uint width, t_uint height, t_uint channels, core::codec::t_colorType colorType) {

		if (!width || !height || !channels) return t_err_InvalidArgument;

		this->_cols = width;
		this->_rows = height;

		this->format.channels = channels;
		this->format.color_type = colorType;
		//this->format.packed = true;
		this->format.bytes_per_pixel = channels;//1 * channels;	// usually 8bit (=1byte)

		t_err err = this->_allocate_memory();

		if (err != t_err_None) return t_err_MemoryAllocationFailed;

		return t_err_None;

	}
	t_err Image::allocate(t_uint width, t_uint height, t_uint channels, core::codec::t_colorType colorType, t_uint depth) {

		if (!width || !height || !channels || !depth) return t_err_InvalidArgument;

		this->_cols = width;
		this->_rows = height;

		this->format.channels = channels;
		this->format.color_type = colorType;
		this->format.bytes_per_pixel = (depth >> 3) * channels;	// usually 8bit (=1byte)

		t_err err = this->_allocate_memory();

		if (err != t_err_None) return t_err_MemoryAllocationFailed;

		return t_err_None;

	}

	void Image::release() {

		Matrix<t_uchar>::release();

		if (this->_external_memory) return;

		this->format.bytes_per_pixel = 0;
		this->format.channels = 0;
		this->format.color_type = core::codec::t_colorType_Unknown;

	}

	t_bool Image::empty() const noexcept {

		return !((this->data != nullptr) | this->_cols | this->_rows | this->format.channels);

	}

	/*
	t_uint64 Image::stride() const noexcept {

		return this->_stride;

	}
	*/
	t_uint Image::depth() const noexcept {

		return (this->format.bytes_per_pixel / this->format.channels) << 3;

	}
	t_uint Image::depthByte() const noexcept {

		return this->format.bytes_per_pixel / this->format.channels;

	}

	/*
	t_err Image::fill(t_uchar c) {

		if (this->empty()) return t_err_MemoryAccessFailed;
		
		memset(this->data, c, this->_stride * this->_rows);

		return t_err_None;

	}
	*/

	/*
	inline t_uint _RGB2BGR(t_uint c) {

		return	(	c & 0xFF00FF00u)	|
				((	c & 0x00FF0000u) >> 16) | 
				((	c & 0x000000FFu) << 16);

	}
	*/
	
	t_bool Image::operator==(const Image& img) const {

		return (
			this->data == img.data &&
			this->_cols == img.width() &&
			this->_rows == img.height() &&
			this->format.channels == img.channels() &&
			this->depth() == img.depth() &&
			this->_stride == img.stride()
			) 
			||
			( this->empty() && img.empty() );

	}

	t_uchar& Image::operator()(t_uint x, t_uint y) {

		return this->data[y * this->_stride + x * this->format.bytes_per_pixel];

	}
	/*
	t_uchar& Image::operator()(t_uint x, t_uint y, t_uint c) {

		return this->data[y * this->_stride + x * this->format.channels + c];

	}
	*/

	t_err decodePNG(Image* dst, const char* filepath) {

		if (!dst->empty())	dst->release();

		core::codec::t_ImageFile_Header in_data;
		t_err err = core::codec::readPNG(&in_data, filepath);
		if (err != t_err_None) return err;

		err = dst->allocate(in_data.width, in_data.height, in_data.channels, in_data.colorType, in_data.depth);
		if (err != t_err_None) return err;

		in_data.stride = dst->stride();
		return core::codec::decodePNG(&in_data, dst->data, filepath);

	}
	t_err encodePNG(Image* src, const char* filepath) {

		if (src->empty()) return t_err_InvalidArgument;

		core::codec::t_ImageFile_Header in_data = {
			src->width(), src->height(), src->stride(), src->channels(), src->colorType(), src->depth()
		};
		return core::codec::writePNG(&in_data, src->data, filepath);

	}

	t_err decodeJPEG(Image* dst, const char* filepath) {
		if (dst == nullptr || filepath == nullptr) return t_err_InvalidArgument;
		if (!dst->empty()) dst->release();

		core::codec::t_ImageFile_Header in_data = {};
		t_err err = core::codec::readJPEG(&in_data, filepath);
		if (err != t_err_None) return err;

		err = dst->allocate(in_data.width, in_data.height, in_data.channels, in_data.colorType, in_data.depth);
		if (err != t_err_None) return err;

		in_data.stride = dst->stride();
		return core::codec::decodeJPEG(&in_data, dst->data, filepath);
	}

	t_err encodeJPEG(Image* src, const char* filepath) {
		if (src == nullptr || src->empty()) return t_err_InvalidArgument;

		core::codec::t_ImageFile_Header in_data = {
			src->width(), src->height(), src->stride(), src->channels(), src->colorType(), src->depth()
		};
		return core::codec::writeJPEG(&in_data, src->data, filepath);
	}

}
