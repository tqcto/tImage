#include "../../include/manage/manage.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <setjmp.h>

#include <png.h>
#include <jpeglib.h>

#define SIGNATURE_NUM	8

namespace tImage {

	struct t_jpeg_error_mgr {
		jpeg_error_mgr pub;
		jmp_buf setjmp_buffer;
	};

	static void jpegErrorExit(j_common_ptr cinfo) {
		auto* error = reinterpret_cast<t_jpeg_error_mgr*>(cinfo->err);
		longjmp(error->setjmp_buffer, 1);
	}

	inline t_err t_fopen(FILE** fpP, const char* filepath, const char* mode) {

#ifdef _WIN32

		return !fopen_s(fpP, filepath, mode) ? t_err_None : t_err_CanNotOpenedFile;


#else

		*fpP = fopen(filepath, mode);
		return *fpP != nullptr ? t_err_None : t_err_CanNotOpenedFile;

#endif

	}

	t_uint64 calcStride4Matrix(t_uint cols, t_uint64 size, t_uint align) {

		// check align
		assert((align & (align - 1)) == 0);

		// bytes per a row. but, this isn't considered align.
		t_uint64 bytes_per_row = static_cast<t_uint64>(cols) * size;

		// calc stride
		return (bytes_per_row + static_cast<t_uint64>(align - 1)) & ~static_cast<t_uint64>(align - 1);

	}
	t_uint64 calcStride(t_uint width, t_uint channels, t_uint depth, t_uint align) {

		// check align
		assert((align & (align - 1)) == 0);

		// bytes per a channel
		//t_uint bytes_per_channel = this->_depth >> 3; // if depth is 8bit, then that is 1byte.

		// bytes per a pixel
		t_uint bytes_per_pixel = channels * (depth >> 3);
		//if (this->format.packed) {
		//bytes_per_pixel = this->format.bytes_per_pixel;
		/* }
		else {

			t_uint* bits = reinterpret_cast<t_uint*>(&this->format.bits_per_channel.r);
			for (int i = 0; i < this->format.channels; i++) {
				bytes_per_pixel += bits[i] >> 3;//(bits[i] + 7) >> 3;
			}

		}*/

		// bytes per a row. but, this isn't considered align.
		t_uint64 bytes_per_row = width * bytes_per_pixel;

		// calc stride
		return (bytes_per_row + (align - 1)) & ~(align - 1);

	}

	t_err readPNG(t_ImageFile_Header* png_header, const char* filepath) {

		FILE* fp = NULL;
		t_err err = t_fopen(&fp, filepath, "rb");
		if (err != t_err_None) {
			printf("%s can't opened.\n", filepath);
			return err;
		}

		png_byte signature[8];
		t_uint read_size = fread(signature, 1, SIGNATURE_NUM, fp);

		if (png_sig_cmp(signature, 0, SIGNATURE_NUM)) {
			printf("png_sig_cmp error!!\n");
			fclose(fp);
			return t_err_CanNotOpenedFile;
		}

		png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (png == NULL) {
			printf("png_create_read_struct error!!\n");
			fclose(fp);
			return t_err_CanNotOpenedFile;
		}

		png_infop info = png_create_info_struct(png);
		if (info == NULL) {
			printf("png_create_info_struct error!!\n");
			fclose(fp);
			return t_err_CanNotOpenedFile;
		}

		png_init_io(png, fp);
		png_set_sig_bytes(png, read_size);
		png_read_png(png, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16, NULL);

		png_header->width		= png_get_image_width(png, info);
		png_header->height		= png_get_image_height(png, info);
		png_header->channels	= png_get_channels(png, info);
		png_header->depth		= png_get_bit_depth(png, info);

		png_byte type = png_get_color_type(png, info);

		switch (type) {
		case PNG_COLOR_TYPE_GRAY:
			png_header->channels = 1;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			png_header->channels = 2;
			break;
		case PNG_COLOR_TYPE_RGB:
			png_header->channels = 3;
			break;
		case PNG_COLOR_TYPE_RGBA:
			png_header->channels = 4;
			break;
		}

		png_destroy_read_struct(&png, &info, NULL);
		fclose(fp);

		return t_err_None;

	}

	t_err decodePNG(t_ImageFile_Header* in_data, t_uchar* dst, const char* filepath) {

		FILE* fp = NULL;
		t_err err = t_fopen(&fp, filepath, "rb");
		if (err != t_err_None) {
			printf("%s can't opened.\n", filepath);
			return err;
		}

		png_byte signature[8];
		t_uint read_size = fread(signature, 1, SIGNATURE_NUM, fp);

		if (png_sig_cmp(signature, 0, SIGNATURE_NUM)) {
			printf("png_sig_cmp error!!\n");
			fclose(fp);
			return t_err_CanNotOpenedFile;
		}

		png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (png == NULL) {
			printf("png_create_read_struct error!!\n");
			fclose(fp);
			return t_err_CanNotOpenedFile;
		}

		png_infop info = png_create_info_struct(png);
		if (info == NULL) {
			printf("png_create_info_struct error!!\n");
			fclose(fp);
			return t_err_CanNotOpenedFile;
		}

		png_init_io(png, fp);
		png_set_sig_bytes(png, read_size);
		png_read_png(png, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16, NULL);

		png_bytepp datap = png_get_rows(png, info);
		
		png_byte type = png_get_color_type(png, info);

		//dst = (t_uchar*)malloc(sizeof(t_uchar) * in_data->width * in_data->height * in_data->channels);
		if (dst == nullptr) {
			png_destroy_read_struct(&png, &info, NULL);
			fclose(fp);
			return t_err_MemoryAllocationFailed;
		}

		for (int i = 0; i < in_data->height; i++) {
			memcpy(dst + i * in_data->stride, datap[i], in_data->width * in_data->channels);
		}

		png_destroy_read_struct(&png, &info, NULL);
		fclose(fp);

		return t_err_None;

	}

	t_err writePNG(t_ImageFile_Header* in_data, t_uchar* src, const char* filepath) {

		FILE* fp;
		t_err err = t_fopen(&fp, filepath, "wb");
		if (err != t_err_None) return err;

		png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		png_infop info = png_create_info_struct(png);

		png_byte type;
		if (in_data->channels == 3) {
			type = PNG_COLOR_TYPE_RGB;
		}
		else if (in_data->channels == 4) {
			type = PNG_COLOR_TYPE_RGBA;
		}
		else {
			printf("channel num is invalid!!\n");
			png_destroy_write_struct(&png, &info);
			fclose(fp);
			return t_err_CanNotWrittenFile;
		}

		png_init_io(png, fp);
		png_set_IHDR(
			png, info, in_data->width, in_data->height, in_data->depth,
			type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
		);

		png_bytepp datap = (png_bytepp)png_malloc(png, sizeof(png_bytep) * in_data->height);

		png_set_rows(png, info, datap);

		for (int i = 0; i < in_data->height; i++) {
			datap[i] = (png_bytep)png_malloc(png, in_data->width * in_data->channels);
			memcpy(datap[i], src + i * in_data->stride, in_data->width * in_data->channels);
		}
		png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

		for (int i = 0; i < in_data->height; i++) {
			png_free(png, datap[i]);
		}
		png_free(png, datap);

		png_destroy_write_struct(&png, &info);
		fclose(fp);
		return t_err_None;

	}

	t_err readJPEG(t_ImageFile_Header* jpeg_header, const char* filepath) {
		if (jpeg_header == nullptr || filepath == nullptr) return t_err_InvalidArgument;

		FILE* fp = nullptr;
		t_err err = t_fopen(&fp, filepath, "rb");
		if (err != t_err_None) return err;

		jpeg_decompress_struct cinfo = {};
		t_jpeg_error_mgr jerr = {};
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = jpegErrorExit;
		volatile bool created = false;
		if (setjmp(jerr.setjmp_buffer)) {
			if (created) jpeg_destroy_decompress(&cinfo);
			fclose(fp);
			return t_err_CanNotOpenedFile;
		}

		jpeg_create_decompress(&cinfo);
		created = true;
		jpeg_stdio_src(&cinfo, fp);
		if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK ||
			(cinfo.jpeg_color_space != JCS_GRAYSCALE &&
			 cinfo.jpeg_color_space != JCS_RGB &&
			 cinfo.jpeg_color_space != JCS_YCbCr)) {
			jpeg_destroy_decompress(&cinfo);
			fclose(fp);
			return t_err_CanNotOpenedFile;
		}

		jpeg_header->width = static_cast<t_uint>(cinfo.image_width);
		jpeg_header->height = static_cast<t_uint>(cinfo.image_height);
		jpeg_header->channels = cinfo.jpeg_color_space == JCS_GRAYSCALE ? 1 : 3;
		jpeg_header->depth = 8;
		jpeg_destroy_decompress(&cinfo);
		fclose(fp);
		return t_err_None;
	}

	t_err decodeJPEG(t_ImageFile_Header* in_data, t_uchar* dst, const char* filepath) {
		if (in_data == nullptr || dst == nullptr || filepath == nullptr) return t_err_InvalidArgument;

		FILE* fp = nullptr;
		t_err err = t_fopen(&fp, filepath, "rb");
		if (err != t_err_None) return err;

		jpeg_decompress_struct cinfo = {};
		t_jpeg_error_mgr jerr = {};
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = jpegErrorExit;
		volatile bool created = false;
		if (setjmp(jerr.setjmp_buffer)) {
			if (created) jpeg_destroy_decompress(&cinfo);
			fclose(fp);
			return t_err_CanNotOpenedFile;
		}

		jpeg_create_decompress(&cinfo);
		created = true;
		jpeg_stdio_src(&cinfo, fp);
		if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK ||
			(cinfo.jpeg_color_space != JCS_GRAYSCALE &&
			 cinfo.jpeg_color_space != JCS_RGB &&
			 cinfo.jpeg_color_space != JCS_YCbCr)) {
			jpeg_destroy_decompress(&cinfo);
			fclose(fp);
			return t_err_CanNotOpenedFile;
		}

		cinfo.out_color_space = cinfo.jpeg_color_space == JCS_GRAYSCALE ? JCS_GRAYSCALE : JCS_RGB;
		jpeg_start_decompress(&cinfo);
		const t_uint channels = static_cast<t_uint>(cinfo.output_components);
		const t_uint64 row_bytes = static_cast<t_uint64>(cinfo.output_width) * channels;
		if (cinfo.output_width != in_data->width || cinfo.output_height != in_data->height ||
			channels != in_data->channels || in_data->stride < row_bytes) {
			jpeg_destroy_decompress(&cinfo);
			fclose(fp);
			return t_err_InvalidArgument;
		}

		JSAMPARRAY row = (*cinfo.mem->alloc_sarray)(
			reinterpret_cast<j_common_ptr>(&cinfo), JPOOL_IMAGE,
			static_cast<JDIMENSION>(row_bytes), 1);
		while (cinfo.output_scanline < cinfo.output_height) {
			const JDIMENSION output_row = cinfo.output_scanline;
			jpeg_read_scanlines(&cinfo, row, 1);
			memcpy(dst + static_cast<t_uint64>(output_row) * in_data->stride, row[0], static_cast<size_t>(row_bytes));
		}
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		fclose(fp);
		return t_err_None;
	}

	t_err writeJPEG(t_ImageFile_Header* in_data, t_uchar* src, const char* filepath) {
		if (in_data == nullptr || src == nullptr || filepath == nullptr ||
			!in_data->width || !in_data->height || in_data->depth != 8 ||
			(in_data->channels != 1 && in_data->channels != 3) ||
			in_data->stride < static_cast<t_uint64>(in_data->width) * in_data->channels) {
			return t_err_InvalidArgument;
		}

		FILE* fp = nullptr;
		t_err err = t_fopen(&fp, filepath, "wb");
		if (err != t_err_None) return err;

		jpeg_compress_struct cinfo = {};
		t_jpeg_error_mgr jerr = {};
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = jpegErrorExit;
		volatile bool created = false;
		if (setjmp(jerr.setjmp_buffer)) {
			if (created) jpeg_destroy_compress(&cinfo);
			fclose(fp);
			return t_err_CanNotWrittenFile;
		}

		jpeg_create_compress(&cinfo);
		created = true;
		jpeg_stdio_dest(&cinfo, fp);
		cinfo.image_width = in_data->width;
		cinfo.image_height = in_data->height;
		cinfo.input_components = static_cast<int>(in_data->channels);
		cinfo.in_color_space = in_data->channels == 1 ? JCS_GRAYSCALE : JCS_RGB;
		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, 90, TRUE);
		jpeg_start_compress(&cinfo, TRUE);
		while (cinfo.next_scanline < cinfo.image_height) {
			JSAMPROW row = src + static_cast<t_uint64>(cinfo.next_scanline) * in_data->stride;
			jpeg_write_scanlines(&cinfo, &row, 1);
		}
		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);
		const bool write_failed = ferror(fp) != 0;
		fclose(fp);
		return write_failed ? t_err_CanNotWrittenFile : t_err_None;
	}

}