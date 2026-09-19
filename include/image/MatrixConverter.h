#pragma once
#include "Matrix.h"
#include "Image.h"

namespace tImage {

    // ImageクラスをMatrixクラスに変換する際に，Matrixクラスのデータ配列を確保するのに必要なメモリサイズを計算．
    // Matrixクラスの型をテンプレートのTで指定．
    // srcのチャンネル数は計算せず，1チャンネルであることを前提とする．
    template<typename T> t_uint64 size_Image2Matrix(Image* src) {

        if (src->empty()) return 0;

        const t_uint64 stride = calcStride4Matrix(src->width(), sizeof(T), src->align());

        return stride * static_cast<t_uint64>(src->height());

    }

    // ImageクラスをMatrixクラスに変換．
    // size_Image2Matrix関数でメモリサイズを計算して，dstのデータ配列を事前に確保しておく必要がある．
    // srcのチャンネル数は1であることを前提とする．
    template<typename T> t_err Image2Matrix(
        Image* __restrict src, Matrix<T>* __restrict dst
    ) {

        if (
            src->empty() || dst->empty()
             || src->width() != dst->cols() || src->height() != dst->rows()
        ) return t_err_InvalidArgument;

        constexpr T scale = static_cast<T>(1) / static_cast<T>(255);

        const t_int width = src->width();
        const t_int height = src->height();

        #pragma omp parallel for schedule(static)
        for (t_int y = 0; y < height; y++) {
            
            const t_uchar* __restrict src_rowptr = src->rowPtr(y);
            T* __restrict dst_rowptr = dst->rowPtr(y);
            
            #pragma omp simd
            for (t_int x = 0; x < width; x++) {

                dst_rowptr[x] = static_cast<T>(src_rowptr[x]) * scale;

            }
        }

        return t_err_None;

    }

    // MatrixクラスをImageクラスに変換．
    // dstのチャンネル数は1であることを前提とする．
    template<typename T> t_err Matrix2Image(
        Matrix<T>* __restrict src, Image* __restrict dst
    ) {

        if (
            src->empty() || dst->empty()
             || src->cols() != dst->width() || src->rows() != dst->height()
        ) return t_err_InvalidArgument;

        const t_int width = dst->width();
        const t_int height = dst->height();

        #pragma omp parallel for schedule(static)
        for (t_int y = 0; y < height; y++) {
            
            const T* __restrict src_rowptr = src->rowPtr(y);
            t_uchar* __restrict dst_rowptr = dst->rowPtr(y);
            
            #pragma omp simd
            for (t_int x = 0; x < width; x++) {

                dst_rowptr[x] = static_cast<t_uchar>(src_rowptr[x] * 255.f);

            }
        }

        return t_err_None;

    }

}
