#pragma once
#include "../tImage_definition.h"
#include <stdio.h>

namespace tImage {
namespace core {

    #define t_CPU_INFO    cpu_info::get()

    enum cpu_vendor : t_uint {

        t_cpu_vendor_Unknown= 0,
        t_cpu_vendor_Intel  = 1 << 0,
        t_cpu_vendor_AMD    = 1 << 1,

    };

    enum cpu_processor : t_uint {

        t_cpu_processor_None  = 0,
        t_cpu_processor_SSE4  = 1 << 0,
        t_cpu_processor_AVX   = 1 << 1,
        t_cpu_processor_AVX2  = 1 << 2,
        t_cpu_processor_AVX512= 1 << 3,

    };

    // MS
    #if defined(T_MS)
    
    // get cpuid
    inline void get_cpuid(t_int* p, t_int i) {
        __cpuid(p, i);
    }

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
        cpu_processor processor;

        cpu_info() {

            // array of information
            // info[0] = EAX
            // info[1] = EBX
            // info[2] = ECX
            // info[3] = EDX
            t_int info[4] = {0};

            // get vendor
            // characts in EBX EDX ECX (little endian)
            // ex: if vendor id is GenuineIntel,
            //      then EBX is 0x756e6547, EDX is 0x49656e69, ECX is 0x6c65746e.
            get_cpuid(info, 0);

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

        }

        // get or generate instance
        inline static const cpu_info& get() {
            static cpu_info instance;
            return instance;
        }

    };

}
}
