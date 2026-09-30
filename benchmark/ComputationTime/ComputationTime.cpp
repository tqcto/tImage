#include <tImage.h>
#include <core/parallel/threadPool.h>

#include <stdio.h>

#include "measure.h"

#define IMG_PATH_PNG "..\\..\\..\\img.png"

void naive_transpose(tImage::Image* src, tImage::Image* dst) {

    for (tImage::t_uint y = 0; y < src->height(); y++) {

        auto src_rowptr = src->rowPtr(y);
        
        for (tImage::t_uint x = 0; x < src->width(); x++) {
            for (tImage::t_uint c = 0; c < src->channels(); c++) {
                
                //dst_rowptr[x * src->channels() + c] = src->rowPtr(x)[y * src->channels() + c];

                dst->rowPtr(x)[y * src->channels() + c] = src_rowptr[x * src->channels() + c];

                // dst->data[x * dst->stride() + y * dst->channels() + c] =
                //     src->data[y * src->stride() + x * src->channels() + c];
            
            }
        }
    }

}

void benchmark_transpose(tImage::Image* src, tImage::Image* dst) {

    printf("transpose benchmark...\n");

    double time_naive = measure([&]() {
        naive_transpose(src, dst);
    });

    double time_rec = measure([&]() {
        tImage::transpose(src, dst);
    });

    printf("naive: %lf ms\n", time_naive);
    printf("optimized: %lf ms\n", time_rec);

}

void benchmark_pfor(tImage::Image* src, tImage::Image* dst) {

    printf("pfor benchmark...\n");

    tImage::t_int width = src->width();
    tImage::t_int height = src->height();
    tImage::t_int channels = src->channels();

    tImage::t_float sum = 0;

    double time_naive = measure([&]() {
        for (tImage::t_int y = 0; y < height; y++) {
            
            auto src_rowptr = src->rowPtr(y);
            auto dst_rowptr = dst->rowPtr(y);
            
            for (tImage::t_int x = 0; x < width; x++) {
                
                sum = 0;

                for (tImage::t_int c = 0; c < channels; c++) {

                    // dst_rowptr[x * channels + c] = src_rowptr[x * channels + c];
                    sum += src_rowptr[x * channels + c];

                }

                sum /= static_cast<tImage::t_float>(channels);
                for (tImage::t_int c = 0; c < channels; c++) {
                    dst_rowptr[x * channels + c] = static_cast<tImage::t_uchar>(sum);
                }

            }

        }
    });

    double time_omp = measure([&]() {
        #pragma omp parallel for
        for (tImage::t_int y = 0; y < height; y++) {
            
            auto src_rowptr = src->rowPtr(y);
            auto dst_rowptr = dst->rowPtr(y);
            
            #pragma omp parallel for
            for (tImage::t_int x = 0; x < width; x++) {
                
                sum = 0;

                for (tImage::t_int c = 0; c < channels; c++) {

                    // dst_rowptr[x * channels + c] = src_rowptr[x * channels + c];
                    sum += src_rowptr[x * channels + c];

                }

                sum /= static_cast<tImage::t_float>(channels);
                for (tImage::t_int c = 0; c < channels; c++) {
                    dst_rowptr[x * channels + c] = static_cast<tImage::t_uchar>(sum);
                }
                
            }

        }
    });

    tImage::core::parallel::threadPool tpool;

    double time_pfor = measure([&]() {
        tpool.pfor(0, height, [src, dst, width, channels](tImage::t_int y) {
            
            auto src_rowptr = src->rowPtr(y);
            auto dst_rowptr = dst->rowPtr(y);
            tImage::t_float sum = 0;

            for (tImage::t_int x = 0; x < width; x++) {
                
                sum = 0;

                for (tImage::t_int c = 0; c < channels; c++) {

                    // dst_rowptr[x * channels + c] = src_rowptr[x * channels + c];
                    sum += src_rowptr[x * channels + c];

                }

                sum /= static_cast<tImage::t_float>(channels);
                for (tImage::t_int c = 0; c < channels; c++) {
                    dst_rowptr[x * channels + c] = static_cast<tImage::t_uchar>(sum);
                }
                
            }

        });
    });

    printf("naive: %lf ms\n", time_naive);
    printf("OpenMP: %lf ms\n", time_omp);
    printf("pfor: %lf ms\n", time_pfor);

}
    
int main() {
    
    /*
    const tImage::t_uint width = 4096;
    const tImage::t_uint height = 4096;

    tImage::Image src(width, height, 3, tImage::core::codec::t_colorType_RGB);
    tImage::Image dst(height, width, 3, tImage::core::codec::t_colorType_RGB);
    */

    tImage::Image src;
    tImage::decodePNG(&src, IMG_PATH_PNG);
    tImage::Image dst(src.height(), src.width(), src.channels(), src.colorType());

    benchmark_transpose(&src, &dst);

    dst.release();
    dst.allocate(src.width(), src.height(), src.channels(), src.colorType());

    benchmark_pfor(&src, &dst);

    return 0;

}
