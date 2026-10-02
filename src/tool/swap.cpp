#include "../../include/tool/swap.h"
#include "../../include/core/cpu.h"
#include "../../include/core/codec/codec.h"
#if defined(TIMAGE_ENABLE_X86_SIMD)
#include "../../include/core/simd/intrin_avx.h"
#include "../../include/core/simd/intrin_ssse3.h"
#endif
#include "../../include/core/parallel/threadPool.h"

// #include <omp.h>

namespace tImage {

    // is mem_align > check_align
    inline t_bool check_big_align(t_uint mem_align, t_uint check_align) {
        return mem_align & (mem_align - 1) &&  mem_align > check_align;
    }

    inline t_uint swap3_pixel(t_uint c) {

		return	(	c & 0x00FF00u)	|           // G
				((	c & 0xFF0000u) >> 16) |     // R
				((	c & 0x0000FFu) << 16);      // B

	}
    inline t_uint swap4_pixel(t_uint c) {

		return	(	c & 0xFF00FF00u)	|       // A, G
				((	c & 0x00FF0000u) >> 16) |   // R
				((	c & 0x000000FFu) << 16);    // B

	}

    inline void swap3(Image* src, Image* dst) {

        auto proc = core::t_CPU_INFO.processor;

        const t_int width = src->width();
        const t_int height = src->height();

        core::parallel::threadPool threadPool;

        // check aligned
        const t_bool check_aligned = check_big_align(src->align(), 16) && check_big_align(dst->align(), 16);

        #if defined(TIMAGE_ENABLE_X86_SIMD)
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

            threadPool.pfor(0, height, [src, dst, loop, mask](t_int y) {
                
                core::simd::v_uint8x16 src_data;

                auto src_rowptr = src->rowPtr(y);
                auto dst_rowptr = dst->rowPtr(y);

                for (t_int x = 0; x < loop; x++) {
                    
                    core::simd::v128_load_8x16(&src_rowptr[x * 5], src_data);

                    core::simd::v128_shuffle_8x16(src_data, src_data, mask);

                    core::simd::v128_store_8x16(src_data, &dst_rowptr[x * 5]);

                }

            });

            // #pragma omp parallel for
            // for (t_int y = 0; y < height; y++) {

            //     core::simd::v_uint8x16 src_data;

            //     auto src_rowptr = src->rowPtr(y);
            //     auto dst_rowptr = dst->rowPtr(y);

            //     for (t_int x = 0; x < loop; x++) {
                    
            //         core::simd::v128_load_8x16(&src_rowptr[x * 5], src_data);

            //         core::simd::v128_shuffle_8x16(src_data, src_data, mask);

            //         core::simd::v128_store_8x16(src_data, &dst_rowptr[x * 5]);

            //     }

            // }

        } else
        #endif
        // normal
        {

            threadPool.pfor(0, height, [src, dst, width](t_int y) {
                
                auto src_rowptr = src->rowPtr(y);
                auto dst_rowptr = dst->rowPtr(y);

                for (t_int x = 0; x < width; x++) {

                    const t_int i = x * 3;
                    const t_int* srcPixelPtr = reinterpret_cast<t_int*>(&src_rowptr[i]);
                    t_int* dstPixelPtr = reinterpret_cast<t_int*>(&dst_rowptr[i]);

                    *dstPixelPtr = swap3_pixel(*srcPixelPtr);
                    
                }

            });

            // #pragma omp parallel for
            // for (t_int y = 0; y < height; y++) {

            //     auto src_rowptr = src->rowPtr(y);
            //     auto dst_rowptr = dst->rowPtr(y);

            //     for (t_int x = 0; x < width; x++) {

            //         const t_int i = x * 3;
            //         const t_int* srcPixelPtr = reinterpret_cast<t_int*>(&src_rowptr[i]);
            //         t_int* dstPixelPtr = reinterpret_cast<t_int*>(&dst_rowptr[i]);

            //         *dstPixelPtr = swap3_pixel(*srcPixelPtr);
                    
            //     }

            // }

        }

    }

    inline void swap4(Image* src, Image* dst) {

        auto proc = core::t_CPU_INFO.processor;

        const t_int width = src->width();
        const t_int height = src->height();

        core::parallel::threadPool threadPool;

        // check aligned
        const t_bool check_aligned = check_big_align(src->align(), 32) && check_big_align(dst->align(), 32);

        #if defined(TIMAGE_ENABLE_X86_SIMD)
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

            threadPool.pfor(0, height, [src, dst, loop, mask](t_int y) {
                
                core::simd::v_uint8x32 src_data;

                auto src_rowptr = src->rowPtr(y);
                auto dst_rowptr = dst->rowPtr(y);

                for (t_int x = 0; x < loop; x++) {
                    
                    core::simd::v256_load_8x32(&src_rowptr[x << 5], src_data);

                    core::simd::v256_shuffle_8x32(src_data, src_data, mask);

                    core::simd::v256_store_8x32(src_data, &dst_rowptr[x << 5]);

                }

            });
            
            // #pragma omp parallel for
            // for (t_int y = 0; y < height; y++) {

            //     core::simd::v_uint8x32 src_data;

            //     auto src_rowptr = src->rowPtr(y);
            //     auto dst_rowptr = dst->rowPtr(y);

            //     for (t_int x = 0; x < loop; x++) {
                    
            //         core::simd::v256_load_8x32(&src_rowptr[x << 5], src_data);

            //         core::simd::v256_shuffle_8x32(src_data, src_data, mask);

            //         core::simd::v256_store_8x32(src_data, &dst_rowptr[x << 5]);

            //     }

            // }

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

            threadPool.pfor(0, height, [src, dst, loop, mask](t_int y) {
                
                core::simd::v_uint8x16 src_data;

                auto src_rowptr = src->rowPtr(y);
                auto dst_rowptr = dst->rowPtr(y);

                for (t_int x = 0; x < loop; x++) {
                    
                    core::simd::v128_load_8x16(&src_rowptr[x << 2], src_data);

                    core::simd::v128_shuffle_8x16(src_data, src_data, mask);

                    core::simd::v128_store_8x16(src_data, &dst_rowptr[x << 2]);

                }

            });
            
            // #pragma omp parallel for
            // for (t_int y = 0; y < height; y++) {

            //     core::simd::v_uint8x16 src_data;

            //     auto src_rowptr = src->rowPtr(y);
            //     auto dst_rowptr = dst->rowPtr(y);

            //     for (t_int x = 0; x < loop; x++) {
                    
            //         core::simd::v128_load_8x16(&src_rowptr[x << 2], src_data);

            //         core::simd::v128_shuffle_8x16(src_data, src_data, mask);

            //         core::simd::v128_store_8x16(src_data, &dst_rowptr[x << 2]);

            //     }

            // }
        } else
        #endif
        // normal
        {

            threadPool.pfor(0, height, [src, dst, width](t_int y) {
                
                auto src_rowptr = src->rowPtr(y);
                auto dst_rowptr = dst->rowPtr(y);

                for (t_int x = 0; x < width; x++) {

                    const t_int i = x << 2;
                    const t_int* srcPixelPtr = reinterpret_cast<t_int*>(&src_rowptr[i]);
                    t_int* dstPixelPtr = reinterpret_cast<t_int*>(&dst_rowptr[i]);

                    *dstPixelPtr = swap4_pixel(*srcPixelPtr);
                    
                }

            });

            // #pragma omp parallel for
            // for (t_int y = 0; y < height; y++) {

            //     auto src_rowptr = src->rowPtr(y);
            //     auto dst_rowptr = dst->rowPtr(y);

            //     for (t_int x = 0; x < width; x++) {

            //         const t_int i = x << 2;
            //         const t_int* srcPixelPtr = reinterpret_cast<t_int*>(&src_rowptr[i]);
            //         t_int* dstPixelPtr = reinterpret_cast<t_int*>(&dst_rowptr[i]);

            //         *dstPixelPtr = swap4_pixel(*srcPixelPtr);
                    
            //     }
                
            // }

        }

    }

    t_err swap(Image* src, Image* dst) noexcept {

        if (src->channels() != dst->channels()) return t_err_InvalidArgument;

        core::codec::t_colorType srcColorType = src->colorType();

        switch (srcColorType)
        {
        case core::codec::t_colorType_RGB:
        case core::codec::t_colorType_BGR:
            swap3(src, dst);
            dst->format.color_type = srcColorType == core::codec::t_colorType_RGB ? core::codec::t_colorType_BGR : core::codec::t_colorType_RGB;
            break;
        
        case core::codec::t_colorType_RGBA:
        case core::codec::t_colorType_BGRA:
            swap4(src, dst);
            dst->format.color_type = srcColorType == core::codec::t_colorType_RGBA ? core::codec::t_colorType_BGRA : core::codec::t_colorType_RGBA;
            break;

        default:
            return t_err_InvalidArgument;    
        
        }

        return t_err_None;

    }
}