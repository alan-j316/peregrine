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

#include <bitset>
#include <cstdint>

namespace pmm
{
    using AllocateTag = uint32_t;

    /// Simple data structure for tracking and allocation usage.
    template <typename Klass, size_t Count>
    struct AllocatorRegistry
    {

        constexpr Klass* get()
        {
            return nullptr;
            // TODO:
        }

        constexpr void free(Klass* klass)
        {
            // TODO:
            // Use pointer offset to calculate the index
            // Turn off the index in the bitmask
        }

        // constexpr Klass&

        [[nodiscard]] constexpr size_t getFirstIndex() const noexcept
        {
            return 0; // TODO:
        }

    private:
        alignas(Klass) uint8_t _data[sizeof(Klass) * Count]{};
        // TODO: Replace with custom bitset to support std::countr_zero for O(1) free bit lookups
        std::bitset<Count> _usageBitmask{};
    };


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


#include "MemoryManager.tpp"
