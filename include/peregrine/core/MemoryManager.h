#pragma once
/**
 * @file MemoryManager.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 29, 2026
 *
 * @brief Centralized memory manager with multithreading support.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "peregrine/allocators/Arena.h"
#include "peregrine/allocators/Pool.h"
#include "peregrine/allocators/Stack.h"
#include "peregrine/allocators/TLSF.h"
#include "peregrine/core/Alias.h"
#include "peregrine/core/PoolRegistry.h"

#include <bitset>
#include <cstdint>


/**
 * @addtogroup PMM_Core
 * @{
 */

namespace pmm
{
    using AllocateTag = uint32_t;


    /// mt    - multithreadable
    /// local - must not be called in a multithreaded environment.
    /// safe  - non-critical checks like nullptr free are guarded against.
    ///         These will not be present in release mode with the "non-safe" variants.

    struct MemoryManagerConfig
    {
        /// <--------- ARENA --------->
        size_t localArenaCount;
        size_t safeLocalArenaCount;
        size_t mtArenaCount;
        size_t safeMtArenaCount;

        /// <--------- STACK --------->
        size_t localStackCount;
        size_t safeLocalStackCount;
        size_t mtStackCount;
        size_t safeMtStackCount;

        /// <--------- POOL --------->
        size_t localPoolCount;
        size_t safeLocalPoolCount;
        size_t mtPoolCount;
        size_t safeMtPoolCount;

        /// <--------- TLSF --------->
        size_t localTLSFCount;
        size_t safeLocalTLSFCount;
        size_t mtTLSFCount;
        size_t safeMtTLSFCount;

        /// <------- TODO: RESERVED WIP --------->
        /// <--------- DOUBLE STACK --------->
        size_t localDStackCount;
        size_t safeLocalDStackCount;
        size_t mtDStackCount;
        size_t safeMtDStackCount;

        /// <------- TODO: RESERVED WIP --------->
        /// <--------- TAGGED POOL STACK --------->
        size_t localTaggedPoolCount;
        size_t safeLocalTaggedPoolCount;
        size_t mtTaggedPoolCount;
        size_t safeMtTaggedPoolCount;
    };

    constexpr MemoryManagerConfig DefaultMemoryManagerConfig{ /// <--------- ARENA --------->
                                                              .localArenaCount     = 16,
                                                              .safeLocalArenaCount = 16,
                                                              .mtArenaCount        = 4,
                                                              .safeMtArenaCount    = 4,

                                                              /// <--------- STACK --------->
                                                              .localStackCount     = 8,
                                                              .safeLocalStackCount = 8,
                                                              .mtStackCount        = 4,
                                                              .safeMtStackCount    = 4,

                                                              /// <--------- POOL --------->
                                                              .localPoolCount     = 128,
                                                              .safeLocalPoolCount = 8,
                                                              .mtPoolCount        = 128,
                                                              .safeMtPoolCount    = 8,

                                                              /// <--------- TLSF --------->
                                                              .localTLSFCount     = 1,
                                                              .safeLocalTLSFCount = 1,
                                                              .mtTLSFCount        = 2,
                                                              .safeMtTLSFCount    = 2,

                                                              /// <------- TODO: RESERVED WIP --------->
                                                              /// <--------- DOUBLE STACK --------->
                                                              .localDStackCount     = 0,
                                                              .safeLocalDStackCount = 0,
                                                              .mtDStackCount        = 0,
                                                              .safeMtDStackCount    = 0,

                                                              /// <------- TODO: RESERVED WIP --------->
                                                              /// <--------- TAGGED POOL STACK --------->
                                                              .localTaggedPoolCount     = 0,
                                                              .safeLocalTaggedPoolCount = 0,
                                                              .mtTaggedPoolCount        = 0,
                                                              .safeMtTaggedPoolCount    = 0
    };

    /// Multithreadable memory manager.
    /// @tparam TelemetryPolicy Telemetry policy used by the memory manager. Enabled by default.
    ///                         Enabling Telemetry can consume more memory and can have a mild impact on performance.
    /// @tparam MemoryPolicy    Dictates whether the buffer is supplied externally or owned internally.
    ///                         Internally managed by default.
    /// @tparam Config          The configuration for the memory manager.
    template <TelPolicy TelemetryPolicy = TelPolicy::Enabled, MemPolicy MemoryPolicy = MemPolicy::Internal,
              MemoryManagerConfig Config = DefaultMemoryManagerConfig>
    class MemoryManager
    {

    public:
        // BLOCK OUT
    private:
        struct MemoryContext* _ctx{};
    };
} // namespace pmm

/** @} */

#include "MemoryManager.tpp"
