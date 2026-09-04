#include <tImage.h>

#include <stdio.h>

#include "measure.h"

void naive_transpose(tImage::Image* src, tImage::Image* dst) {

    for (tImage::t_uint y = 0; y < src->height(); y++) {
        for (tImage::t_uint x = 0; x < src->width(); x++) {
            for (tImage::t_uint c = 0; c < src->channels(); c++) {
                dst->data[x * dst->stride() + y * dst->channels() + c] =
                    src->data[y * src->stride() + x * src->channels() + c];
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
    
int main() {
    
    const tImage::t_uint width = 4096;
    const tImage::t_uint height = 4096;

    tImage::Image src(width, height, 3);
    tImage::Image dst(height, width, 3);

    benchmark_transpose(&src, &dst);

    return 0;

}
