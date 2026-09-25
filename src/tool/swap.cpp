#include "../../include/tool/swap.h"
#include "../../include/core/cpu.h"
#include "../../include/core/simd/intrin_avx.h"
#include "../../include/core/simd/intrin_ssse3.h"

#include <omp.h>

namespace tImage {

    // is mem_align > check_align
    inline t_bool check_big_align(t_uint mem_align, t_uint check_align) {
        return mem_align & (mem_align - 1) &&  mem_align > check_align;
    }

    inline void swap3(Image* src, Image* dst) {

        auto proc = core::t_CPU_INFO.processor;

        const t_int width = src->width();
        const t_int height = src->height();

        // check aligned
        const t_bool check_aligned = check_big_align(src->align(), 16) && check_big_align(dst->align(), 16);

        // simd 128 bit
        if (proc & core::t_cpu_processor_SSSE3 && check_aligned) {

            const t_int loop = 3 * (width >> 4);

            const core::simd::v_uint8x16 mask(
                2, 1, 0,
                5, 4, 3,
                8, 7, 6,
                11, 10, 9,
                14, 13, 12,
                15
            );

            #pragma omp parallel for
            for (t_int y = 0; y < height; y++) {

                core::simd::v_uint8x16 src_data;

                auto src_rowptr = src->rowPtr(y);
                auto dst_rowptr = dst->rowPtr(y);

                for (t_int x = 0; x < loop; x++) {
                    
                    core::simd::v128_load_8x16(&src_rowptr[x * 5], src_data);

                    core::simd::v128_shuffle_8x16(src_data, src_data, mask);

                    core::simd::v128_store_8x16(src_data, &dst_rowptr[x * 5]);

                }

            }

        }
        // normal
        else {

            #pragma omp parallel for
            for (t_int y = 0; y < height; y++) {

                auto src_rowptr = src->rowPtr(y);
                auto dst_rowptr = dst->rowPtr(y);

                for (t_int x = 0; x < width; x++) {

                    const t_int i = x * 3;

                    dst_rowptr[i] = src_rowptr[i + 2];
                    dst_rowptr[i + 1] = src_rowptr[i + 1];
                    dst_rowptr[i + 2] = src_rowptr[i];

                }

            }

        }

    }

    inline void swap4(Image* src, Image* dst) {

        auto proc = core::t_CPU_INFO.processor;

        const t_int width = src->width();
        const t_int height = src->height();

        // check aligned
        const t_bool check_aligned = check_big_align(src->align(), 32) && check_big_align(dst->align(), 32);

        // simd 256 bit
        if (proc & core::t_cpu_processor_AVX && check_aligned) {

            const t_int loop = src->elementsRow() >> 5;

            const core::simd::v_uint8x32 mask(
                2, 1, 0, 3,
                6, 5, 4, 7,
                10, 9, 8, 11,
                14, 13, 12, 15,
                18, 17, 16, 19,
                22, 21, 20, 23,
                26, 25, 24, 27,
                30, 29, 28, 31
            );
            
            #pragma omp parallel for
            for (t_int y = 0; y < height; y++) {

                core::simd::v_uint8x32 src_data;

                auto src_rowptr = src->rowPtr(y);
                auto dst_rowptr = dst->rowPtr(y);

                for (t_int x = 0; x < loop; x++) {
                    
                    core::simd::v256_load_8x32(&src_rowptr[x << 5], src_data);

                    core::simd::v256_shuffle_8x32(src_data, src_data, mask);

                    core::simd::v256_store_8x32(src_data, &dst_rowptr[x << 5]);

                }

            }

        }
        // simd 128 bit
        if (proc & core::t_cpu_processor_SSSE3 && check_aligned) {

            const t_int loop = src->elementsRow() >> 2;

            const core::simd::v_uint8x16 mask(
                2, 1, 0, 3,
                6, 5, 4, 7,
                10, 9, 8, 11,
                14, 13, 12, 15
            );
            
            #pragma omp parallel for
            for (t_int y = 0; y < height; y++) {

                core::simd::v_uint8x16 src_data;

                auto src_rowptr = src->rowPtr(y);
                auto dst_rowptr = dst->rowPtr(y);

                for (t_int x = 0; x < loop; x++) {
                    
                    core::simd::v128_load_8x16(&src_rowptr[x << 2], src_data);

                    core::simd::v128_shuffle_8x16(src_data, src_data, mask);

                    core::simd::v128_store_8x16(src_data, &dst_rowptr[x << 2]);

                }

            }
        }
        // normal
        else {

            #pragma omp parallel for
            for (t_int y = 0; y < height; y++) {

                auto src_rowptr = src->rowPtr(y);
                auto dst_rowptr = dst->rowPtr(y);

                for (t_int x = 0; x < width; x++) {

                    const t_int i = x << 2;

                    dst_rowptr[i] = src_rowptr[i + 2];
                    dst_rowptr[i + 1] = src_rowptr[i + 1];
                    dst_rowptr[i + 2] = src_rowptr[i];
                    dst_rowptr[i + 3] = src_rowptr[i + 3];

                }

            }

        }

    }

    t_err swap(Image* src, Image* dst) {

        if (src->channels() != dst->channels()) return t_err_InvalidArgument;

        switch (src->channels())
        {
        case 3:
            swap3(src, dst);
            break;
        
        case 4:
            swap4(src, dst);
            break;

        default:
            return t_err_InvalidArgument;    
        
        }

        return t_err_None;

    }
}