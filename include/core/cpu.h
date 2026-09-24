#pragma once
#include "../tImage_definition.h"

#if defined(T_MS)

#include <intrin.h>

#elif defined(T_GCC)

#include <cpuid.h>

#endif

namespace tImage {
namespace core {

    #define t_CPU_INFO    cpu_info::get()

    enum cpu_vendor : t_uint {

        t_cpu_vendor_Unknown= 0,
        t_cpu_vendor_Intel  = 1 << 0,
        t_cpu_vendor_AMD    = 1 << 1,

    };

    enum cpu_processor : t_uint {

        t_cpu_processor_None    = 0,
        t_cpu_processor_SSE     = 1 << 0,
        t_cpu_processor_SSE2    = 1 << 1,
        t_cpu_processor_SSE3    = 1 << 2,
        t_cpu_processor_SSSE3   = 1 << 3,
        t_cpu_processor_SSE4_1  = 1 << 4,
        t_cpu_processor_SSE4_2  = 1 << 5,
        t_cpu_processor_AVX     = 1 << 6,
        t_cpu_processor_AVX2    = 1 << 7,
        t_cpu_processor_AVX512f = 1 << 8,
        t_cpu_processor_AVX512dq= 1 << 9,

    };

    // MS
    #if defined(T_MS)
    
    // get cpuid
    inline void get_cpuid(t_int* p, t_int i) {
        __cpuid(p, i);
    }

    extern "C" DLL_EXPORT void cpuid_msvc_x64(t_int* p, t_int i);

    // get cpuidex
    inline void get_cpuidex(t_int* p, t_int i, t_int c) {
        __cpuidex(p, i, c);
    }

    // GCC
    #elif defined(T_GCC)

    // get cpuid
    inline void get_cpuid(t_int* p, t_int i) {
        __cpuid(i, p[0], p[1], p[2], p[3]);
    }

    // get cpuidex
    inline void get_cpuidex(t_int* p, t_int i, t_int c) {
        __cpuid_count(i, c, p[0], p[1], p[2], p[3]);
    }

    #endif

    // information of cpu
    struct cpu_info {

        cpu_vendor vendor;
        t_uint processor;

        cpu_info() : vendor{t_cpu_vendor_Unknown}, processor{t_cpu_processor_None} {

            // array of information
            // info[0] = EAX
            // info[1] = EBX
            // info[2] = ECX
            // info[3] = EDX
            t_int info[4] = {0};

            constexpr t_int EAX = 0;
            constexpr t_int EBX = 1;
            constexpr t_int ECX = 2;
            constexpr t_int EDX = 3;

            // get vendor
            // characts in EBX EDX ECX (little endian)
            // ex: if vendor id is GenuineIntel,
            //      then EBX is 0x756e6547, EDX is 0x49656e69, ECX is 0x6c65746e.
            get_cpuid(info, 0);
            // cpuid_msvc_x64(info, 0);

            constexpr t_int vendor_intel_ebx = 0x756e6547; // uneG
            constexpr t_int vendor_intel_edx = 0x49656e69; // Ieni
            constexpr t_int vendor_intel_ecx = 0x6c65746e; // letn

            constexpr t_int vendor_amd_ebx = 0x68747541; // htuA
            constexpr t_int vendor_amd_edx = 0x69746e65; // itne
            constexpr t_int vendor_amd_ecx = 0x444d4163; // DMAc

            if (
                info[1] == vendor_intel_ebx
                && info[2] == vendor_intel_ecx
                && info[3] == vendor_intel_edx
            ) {
                this->vendor = t_cpu_vendor_Intel;
            }
            else if (
                info[1] == vendor_amd_ebx
                && info[2] == vendor_amd_ecx
                && info[3] == vendor_amd_edx
            ) {
                this->vendor = t_cpu_vendor_AMD;
            }
            else {
                this->vendor = t_cpu_vendor_Unknown;
            }
        
            // get processors
            get_cpuid(info, 1);

            // SSE
            if (info[EDX] & (1 << 25)) {
                this->processor |= t_cpu_processor_SSE;
            }
            // SSE2
            if (info[EDX] & (1 << 26)) {
                this->processor |= t_cpu_processor_SSE2;
            }
            // SSE3
            if (info[ECX] == 0) {
                this->processor |= t_cpu_processor_SSE3;
            }
            // SSSE3
            if (info[ECX] & (1 << 9)) {
                this->processor |= t_cpu_processor_SSSE3;
            }
            // SSE4.1
            if (info[ECX] & (1 << 19)) {
                this->processor |= t_cpu_processor_SSE4_1;
            }
            // SSE4.2
            if (info[ECX] & (1 << 20)) {
                this->processor |= t_cpu_processor_SSE4_2;
            }

            const bool os_supports_avx =
                (info[ECX] & (1 << 27))
                && (info[ECX] & (1 << 28)
            );

            // AVX
            if (os_supports_avx) {
                this->processor |= t_cpu_processor_AVX;
            }

            // get extensions
            get_cpuidex(info, 7, 0);

            // AVX2
            if (os_supports_avx && (info[EBX] & (1 << 5))) {
                this->processor |= t_cpu_processor_AVX2;
            }
            // AVX512-f
            if (os_supports_avx && (info[EBX] & (1 << 16))) {
                this->processor |= t_cpu_processor_AVX512f;
            }
            // AVX512-dq
            if (os_supports_avx && (info[EBX] & (1 << 17))) {
                this->processor |= t_cpu_processor_AVX512dq;
            }

        }

        // get or generate instance
        inline static const cpu_info& get() {
            static cpu_info instance;
            return instance;
        }

    };

}
}
