#include "../../../include/core/codec/jpeg.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <setjmp.h>

#include <jpeglib.h>

namespace tImage {
namespace core {
namespace codec {

	struct t_jpeg_error_mgr {
		jpeg_error_mgr pub;
		jmp_buf setjmp_buffer;
	};

	static void jpegErrorExit(j_common_ptr cinfo) {
		auto* error = reinterpret_cast<t_jpeg_error_mgr*>(cinfo->err);
		longjmp(error->setjmp_buffer, 1);
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
}
}