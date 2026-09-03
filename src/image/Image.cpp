#include "../../include/image/Image.h"
#include "../../include/manage/manage.h"

#include <stdlib.h>
#include <assert.h> // for assert
#include <immintrin.h>

namespace tImage {

	t_err Image::_allocate_memory() {

		// calc stride
		this->_stride = calcStride(this->_width, this->format.channels, this->depth(), this->_align);

		// total bytes
		t_uint64 total_bytes = this->_stride * (t_uint64)this->_height;

		this->data = (t_uchar*)malloc(total_bytes);

		return this->data != nullptr ? t_err_None : t_err_MemoryAllocationFailed;

	}

	Image::Image(void) {
	
		

	}
	Image::Image(t_uint width, t_uint height, t_uint channels) {

		t_err err = this->allocate(width, height, channels);

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
	Image::Image(t_uint width, t_uint height, t_uint channels, t_uint depth) {

		t_err err = this->allocate(width, height, channels, depth);

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

	t_err Image::setAlign(t_uint align) {

		// 2�ׂ̂��悩����
		if (align & (align - 1) && !(this->data)) return t_err_InvalidArgument;
		
		this->_align = align;
		return t_err_None;

	}

	t_err Image::input(
		t_uchar* src,
		t_uint width, t_uint height,
		t_uint channels, t_uint depth,
		t_uint align
	) {

		t_flags err = t_err_None;

		err = this->setAlign(align);
		if (err != t_err_None) return err;

		err = !width || !height || !channels ? t_err_InvalidArgument : t_err_None;

		this->_width = width;
		this->_height = height;

		this->format.channels = channels;
		this->format.bytes_per_pixel = (depth >> 3) * channels;	// usually 8bit (=1byte)
		
		err |= this->setAlign(align);
		if (err != t_err_None) return err;

		this->_stride = calcStride(width, channels, depth, _align);

		this->data = src;
		this->_external_memory = true;

		return err;

	}

	t_err Image::allocate(t_uint width, t_uint height, t_uint channels) {

		if (!width || !height || !channels) return t_err_InvalidArgument;

		this->_width = width;
		this->_height = height;

		this->format.channels = channels;
		//this->format.packed = true;
		this->format.bytes_per_pixel = channels;//1 * channels;	// usually 8bit (=1byte)

		t_err err = this->_allocate_memory();

		if (err != t_err_None) return t_err_MemoryAllocationFailed;

	}
	t_err Image::allocate(t_uint width, t_uint height, t_uint channels, t_uint depth) {

		if (!width || !height || !channels || !depth) return t_err_InvalidArgument;

		this->_width = width;
		this->_height = height;

		this->format.channels = channels;
		this->format.bytes_per_pixel = (depth >> 3) * channels;	// usually 8bit (=1byte)

		t_err err = this->_allocate_memory();

		if (err != t_err_None) return t_err_MemoryAllocationFailed;

	}

	void Image::release() {

		if (this->_external_memory) {
		
			//throw "External memory can't be release.";
			return;
		
		}

		free(this->data);
		this->data = nullptr;
		this->_external_memory = false;

		this->_align = T_IMAGE_DEFAULT_ALIGN;

		this->_width = 0;
		this->_height = 0;
		this->_stride = 0;
		this->format.bytes_per_pixel = 0;
		this->format.channels = 0;

	}

	t_bool Image::empty() const noexcept {

		return !(this->data != nullptr | this->_width | this->_height | this->format.channels);

	}

	t_uint Image::width() const noexcept {

		return this->_width;

	}
	t_uint Image::height() const noexcept {

		return this->_height;

	}
	t_uint64 Image::stride() const noexcept {

		return this->_stride;

	}
	t_uint Image::channels() const noexcept {

		return this->format.channels;

	}
	t_uint Image::depth() const noexcept {

		return (this->format.bytes_per_pixel / this->format.channels) << 3;

	}
	t_uint Image::depthByte() const noexcept {

		return this->format.bytes_per_pixel / this->format.channels;

	}
	/*
	t_colorType Image::colorType() const noexcept {

		return this->_colorType;

	}
	*/

	inline t_uint _RGB2BGR(t_uint c) {

		return	(	c & 0xFF00FF00u)	|
				((	c & 0x00FF0000u) >> 16) | 
				((	c & 0x000000FFu) << 16);

	}
	t_err Image::RGB2BGR(void) {

		if (this->format.channels < 3) return t_err_MemoryAccessFailed;

		// only handle 8bit per channel here
		if (this->depthByte() != 1) {
			// fallback: simple byte-swap scalar
			t_uint64 pixels = (t_uint64)this->_width * this->_height;
			t_uchar* p = this->data;
			for (t_uint64 i = 0; i < pixels; ++i) {
				t_uchar r = p[3*i + 0];
				p[3*i + 0] = p[3*i + 2];
				p[3*i + 2] = r;
			}
			return t_err_None;
		}

		const t_uint64 pixels = (t_uint64)this->_width * this->_height;
		t_uchar* src = this->data;

		// temporary 32-bit buffer (one uint32 per pixel: low 3 bytes used)
		t_uint* tmp = (t_uint*)malloc(pixels * sizeof(t_uint));
		if (!tmp) return t_err_MemoryAllocationFailed;

		// expand RGB24 -> 32bit words (B,G,R,0 in low bytes layout or R|G<<8|B<<16)
		for (t_uint64 i = 0; i < pixels; ++i) {
			tmp[i] = (t_uint)src[3*i + 0] | ((t_uint)src[3*i + 1] << 8) | ((t_uint)src[3*i + 2] << 16);
		}

		// AVX2: swap R and B in 32-bit lanes:
		// swapped = (val & 0xFF00FF00) | ((val & 0x000000FF) << 16) | ((val & 0x00FF0000) >> 16)
		const __m256i m_keep = _mm256_set1_epi32(0xFF00FF00u);
		const __m256i m_r    = _mm256_set1_epi32(0x000000FFu);
		const __m256i m_b    = _mm256_set1_epi32(0x00FF0000u);

		t_uint64 i = 0;
		for (; i + 8 <= pixels; i += 8) {
			__m256i v = _mm256_loadu_si256((const __m256i*)(tmp + i));
			__m256i vr = _mm256_and_si256(v, m_r);
			__m256i vb = _mm256_and_si256(v, m_b);
			vr = _mm256_slli_epi32(vr, 16);
			vb = _mm256_srli_epi32(vb, 16);
			v = _mm256_and_si256(v, m_keep);
			v = _mm256_or_si256(v, vr);
			v = _mm256_or_si256(v, vb);
			_mm256_storeu_si256((__m256i*)(tmp + i), v);
		}
		// tail
		for (; i < pixels; ++i) {
			tmp[i] = _RGB2BGR(tmp[i]);
		}

		// pack back: 32bit -> RGB24
		for (t_uint64 j = 0; j < pixels; ++j) {
			t_uint v = tmp[j];
			src[3*j + 0] = (t_uchar)(v & 0xFFu);
			src[3*j + 1] = (t_uchar)((v >> 8) & 0xFFu);
			src[3*j + 2] = (t_uchar)((v >> 16) & 0xFFu);
		}

		free(tmp);
		return t_err_None;
		
	}
	
	t_err Image::RGBA2BGRA(void) {

		if (this->format.channels < 4) return t_err_MemoryAccessFailed;

		if (this->depthByte() != 1) return t_err_MemoryAccessFailed;

		// mask
		const __m256i m_keep = _mm256_set1_epi32(0xFF00FF00u); // keep G and A
		const __m256i m_r    = _mm256_set1_epi32(0x000000FFu);
		const __m256i m_b    = _mm256_set1_epi32(0x00FF0000u);

		t_uint64 i = 0;
		for (; i + 8 <= this->stride() * this->_height; i += 8) {

			__m256i v = _mm256_loadu_si256((const __m256i*)(this->data + i * 4));
			__m256i vr = _mm256_and_si256(v, m_r);
			__m256i vb = _mm256_and_si256(v, m_b);
			vr = _mm256_slli_epi32(vr, 16);
			vb = _mm256_srli_epi32(vb, 16);
			v = _mm256_and_si256(v, m_keep);
			v = _mm256_or_si256(v, vr);
			v = _mm256_or_si256(v, vb);
			_mm256_storeu_si256((__m256i*)(this->data + i * 4), v);

		}

	}

	t_bool Image::operator==(const Image& img) const {

		return (
			this->data == img.data &&
			this->_width == img.width() &&
			this->_height == img.height() &&
			this->format.channels == img.channels() &&
			this->depth() == img.depth() &&
			this->_stride == img.stride()
			) 
			||
			( this->empty() && img.empty() );

	}

	t_err decodePNG(Image* dst, const char* filepath) {

		if (!dst->empty())	dst->release();

		t_ImageFile_Header in_data;
		t_err err = readPNG(&in_data, filepath);
		if (err != t_err_None) return err;

		err = dst->allocate(in_data.width, in_data.height, in_data.channels, in_data.depth);
		if (err != t_err_None) return err;

		in_data.stride = dst->stride();
		return decodePNG(&in_data, dst->data, filepath);

	}
	t_err encodePNG(Image* src, const char* filepath) {

		if (src->empty()) return t_err_InvalidArgument;

		t_ImageFile_Header in_data = {
			src->width(), src->height(), src->stride(), src->channels(), src->depth()
		};
		return writePNG(&in_data, src->data, filepath);

	}

}
