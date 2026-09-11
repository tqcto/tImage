#pragma once
#include "../tImage_definition.h"
#include "../manage/manage.h"

#include <stdlib.h>

namespace tImage {

    template <typename T>
    class Matrix {

	protected:

		t_uint _align = T_IMAGE_DEFAULT_ALIGN;	// 32byte align

		t_uint _cols = 0;
		t_uint _rows = 0;
		t_uint64 _stride = 0;		// length of a row
		//t_uint _channels = 8;
		//t_uint _depth = 8;		// Color depth. usually 8.
		//PixelFormat format;		// pixel format
		//t_colorType _colorType = t_colorType_RGB;

		// Is memory from external
		t_bool _external_memory = false;

		t_err _allocate_memory() {

            // calc stride
            t_uint64 stride_bytes = calcStride4Matrix(
                this->_cols * static_cast<t_uint>(sizeof(T)), this->_align);
            this->_stride = (stride_bytes + sizeof(T) - 1) / sizeof(T);

            // total bytes
            t_uint64 total_bytes = this->_stride * this->_rows * sizeof(T);

            this->data = (T*)malloc(total_bytes);

            return this->data != nullptr ? t_err_None : t_err_MemoryAllocationFailed;

        }

	public:

		// image data
		T* data = nullptr;

		// initialize class

		/* Initialize empty class */
		DLL_EXPORT Matrix(void) {

        };
		/* Initialize class from allocate function */
		DLL_EXPORT Matrix(t_uint cols, t_uint rows) {

            t_err err = this->allocate(cols, rows);

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

		DLL_EXPORT ~Matrix() {

            if (this->data) {
                this->release();
            }

        }

		/* Set align. Only powers of 2 can be specified. */
		DLL_EXPORT t_err setAlign(t_uint align) {

            if (align & (align - 1) && !(this->data)) return t_err_InvalidArgument;
            
            this->_align = align;
            return t_err_None;

        }

		/* Input image of other memory*/
		DLL_EXPORT t_err input(
			T* src,
			t_uint cols, t_uint rows,
			t_uint align
		) {

            t_flags err = t_err_None;

            err = this->setAlign(align);
            if (err != t_err_None) return err;

            err = !cols || !rows ? t_err_InvalidArgument : t_err_None;

            this->_cols = cols;
            this->_rows = rows;

            /*
            err |= this->setAlign(align);
            if (err != t_err_None) return err;
            */

            t_uint64 stride_bytes = calcStride4Matrix(
                cols * static_cast<t_uint>(sizeof(T)), _align);
            this->_stride = (stride_bytes + sizeof(T) - 1) / sizeof(T);

            this->data = src;
            this->_external_memory = true;

            return err;

        }

		/* Allocate memory of image */
		DLL_EXPORT t_err allocate(t_uint cols, t_uint rows) {

            if (!cols || !rows) return t_err_InvalidArgument;

            this->_cols = cols;
            this->_rows = rows;

            t_err err = this->_allocate_memory();

            if (err != t_err_None) return t_err_MemoryAllocationFailed;

            return t_err_None;

        }

		/* Release memory */
		DLL_EXPORT void release() {

            if (this->_external_memory) {
            
                //throw "External memory can't be release.";
                return;
            
            }

            free(this->data);
            this->data = nullptr;
            this->_external_memory = false;

            //this->_align = T_IMAGE_DEFAULT_ALIGN;

            this->_cols = 0;
            this->_rows = 0;
            this->_stride = 0;

        }

		/* Get whether class is empty. If empty then returned true. */
		DLL_EXPORT t_bool empty() const noexcept {

            return !(this->data != nullptr | this->_cols | this->_rows);

        }

		/* Get width of image */
		DLL_EXPORT t_uint cols() const noexcept {
            return this->_cols;
        }
		/* Get height of image */
		DLL_EXPORT t_uint rows() const noexcept {
            return this->_rows;
        }
		/* Get stride of image */
		DLL_EXPORT t_uint64 stride() const noexcept {
            return this->_stride;
        }

        // 書き込み用
        DLL_EXPORT T& operator()(t_uint col, t_uint row) {

        return this->data[row * this->_stride + col];

        }

	};

}