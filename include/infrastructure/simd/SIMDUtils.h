/**
 * @file SIMDUtils.h
 * @brief Cross-platform SIMD utilities with fallbacks
 * 
 * Provides a unified interface for SIMD operations across different
 * architectures (x86 SSE/AVX, ARM NEON) with scalar fallbacks.
 * 
 * @author FIX-FastTrade Team
 * @date 2025
 */

#ifndef INFRASTRUCTURE_SIMD_UTILS_H
#define INFRASTRUCTURE_SIMD_UTILS_H

#include "infrastructure/platform/Platform.h"
#include <cstring>
#include <algorithm>

// Platform-specific SIMD headers
#if ARCH_X86_64 || defined(ARCH_X86_32)
    #ifdef HAVE_SSE2
        #include <emmintrin.h>  // SSE2
    #endif
    #ifdef HAVE_AVX2
        #include <immintrin.h>  // AVX2
    #endif
#elif ARCH_ARM64
    #ifdef HAVE_NEON
        #include <arm_neon.h>
    #endif
#endif

namespace infrastructure {
namespace simd {

/**
 * @brief SIMD-optimized memory search for single byte
 * 
 * Searches for the first occurrence of a byte in a memory buffer
 * using SIMD instructions when available, with scalar fallback.
 * 
 * @param haystack Buffer to search in
 * @param needle Byte to search for
 * @param length Length of buffer
 * @return Pointer to first occurrence or nullptr if not found
 */
inline const char* findByte(const char* haystack, char needle, size_t length) {
    if (!haystack || length == 0) {
        return nullptr;
    }

#if ARCH_X86_64 && defined(HAVE_SSE2)
    // SSE2 implementation
    const __m128i needle_vec = _mm_set1_epi8(needle);
    const char* end = haystack + length;
    const char* aligned_end = haystack + (length & ~15);  // 16-byte aligned end
    
    // Process 16 bytes at a time
    for (const char* ptr = haystack; ptr < aligned_end; ptr += 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
        __m128i cmp = _mm_cmpeq_epi8(chunk, needle_vec);
        int mask = _mm_movemask_epi8(cmp);
        
        if (mask != 0) {
            // Found match, find exact position
            int pos = __builtin_ctz(mask);
            return ptr + pos;
        }
    }
    
    // Handle remaining bytes
    for (const char* ptr = aligned_end; ptr < end; ++ptr) {
        if (*ptr == needle) {
            return ptr;
        }
    }
    
#elif ARCH_ARM64 && defined(HAVE_NEON)
    // ARM NEON implementation
    const uint8x16_t needle_vec = vdupq_n_u8(static_cast<uint8_t>(needle));
    const char* end = haystack + length;
    const char* aligned_end = haystack + (length & ~15);  // 16-byte aligned end
    
    // Process 16 bytes at a time
    for (const char* ptr = haystack; ptr < aligned_end; ptr += 16) {
        uint8x16_t chunk = vld1q_u8(reinterpret_cast<const uint8_t*>(ptr));
        uint8x16_t cmp = vceqq_u8(chunk, needle_vec);
        
        // Check if any bytes matched
        uint64x2_t cmp64 = vreinterpretq_u64_u8(cmp);
        if (vgetq_lane_u64(cmp64, 0) != 0 || vgetq_lane_u64(cmp64, 1) != 0) {
            // Found match, find exact position
            for (int i = 0; i < 16; ++i) {
                if (ptr[i] == needle) {
                    return ptr + i;
                }
            }
        }
    }
    
    // Handle remaining bytes
    for (const char* ptr = aligned_end; ptr < end; ++ptr) {
        if (*ptr == needle) {
            return ptr;
        }
    }
    
#else
    // Scalar fallback
    const char* result = static_cast<const char*>(std::memchr(haystack, needle, length));
    return result;
#endif

    return nullptr;
}

/**
 * @brief SIMD-optimized checksum calculation
 * 
 * Calculates a simple checksum using SIMD when available.
 * 
 * @param data Buffer to checksum
 * @param length Length of buffer
 * @return Checksum value
 */
inline uint32_t calculateChecksum(const char* data, size_t length) {
    if (!data || length == 0) {
        return 0;
    }

#if ARCH_X86_64 && defined(HAVE_SSE2)
    // SSE2 implementation
    __m128i sum = _mm_setzero_si128();
    const char* end = data + length;
    const char* aligned_end = data + (length & ~15);
    
    // Process 16 bytes at a time
    for (const char* ptr = data; ptr < aligned_end; ptr += 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
        __m128i chunk_u16_lo = _mm_unpacklo_epi8(chunk, _mm_setzero_si128());
        __m128i chunk_u16_hi = _mm_unpackhi_epi8(chunk, _mm_setzero_si128());
        sum = _mm_add_epi16(sum, chunk_u16_lo);
        sum = _mm_add_epi16(sum, chunk_u16_hi);
    }
    
    // Horizontal sum
    sum = _mm_add_epi16(sum, _mm_srli_si128(sum, 8));
    sum = _mm_add_epi16(sum, _mm_srli_si128(sum, 4));
    sum = _mm_add_epi16(sum, _mm_srli_si128(sum, 2));
    
    uint32_t result = _mm_extract_epi16(sum, 0);
    
    // Handle remaining bytes
    for (const char* ptr = aligned_end; ptr < end; ++ptr) {
        result += static_cast<uint8_t>(*ptr);
    }
    
    return result;
    
#elif ARCH_ARM64 && defined(HAVE_NEON)
    // ARM NEON implementation
    uint32x4_t sum = vdupq_n_u32(0);
    const char* end = data + length;
    const char* aligned_end = data + (length & ~15);
    
    // Process 16 bytes at a time
    for (const char* ptr = data; ptr < aligned_end; ptr += 16) {
        uint8x16_t chunk = vld1q_u8(reinterpret_cast<const uint8_t*>(ptr));
        uint16x8_t chunk_u16_lo = vmovl_u8(vget_low_u8(chunk));
        uint16x8_t chunk_u16_hi = vmovl_u8(vget_high_u8(chunk));
        sum = vaddw_u16(sum, vget_low_u16(chunk_u16_lo));
        sum = vaddw_u16(sum, vget_high_u16(chunk_u16_lo));
        sum = vaddw_u16(sum, vget_low_u16(chunk_u16_hi));
        sum = vaddw_u16(sum, vget_high_u16(chunk_u16_hi));
    }
    
    // Horizontal sum
    uint32_t result = vaddvq_u32(sum);
    
    // Handle remaining bytes
    for (const char* ptr = aligned_end; ptr < end; ++ptr) {
        result += static_cast<uint8_t>(*ptr);
    }
    
    return result;
    
#else
    // Scalar fallback
    uint32_t sum = 0;
    for (size_t i = 0; i < length; ++i) {
        sum += static_cast<uint8_t>(data[i]);
    }
    return sum;
#endif
}

/**
 * @brief Check if SIMD is available at runtime
 */
inline bool isSIMDAvailable() {
#if defined(HAVE_SSE2) || defined(HAVE_NEON)
    return true;
#else
    return false;
#endif
}

/**
 * @brief Get SIMD capability string
 */
inline const char* getSIMDCapabilities() {
#if ARCH_X86_64 || defined(ARCH_X86_32)
    #ifdef HAVE_AVX512
        return "AVX512";
    #elif defined(HAVE_AVX2)
        return "AVX2";
    #elif defined(HAVE_SSE2)
        return "SSE2";
    #else
        return "None";
    #endif
#elif ARCH_ARM64
    #ifdef HAVE_NEON
        return "NEON";
    #else
        return "None";
    #endif
#else
    return "None";
#endif
}

} // namespace simd
} // namespace infrastructure

#endif // INFRASTRUCTURE_SIMD_UTILS_H