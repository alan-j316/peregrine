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


    struct MemoryManagerConfig
    {

        // TODO:
    };

    constexpr MemoryManagerConfig DefaultMemoryManagerConfig{};

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
