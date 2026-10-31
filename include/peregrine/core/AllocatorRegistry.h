#pragma once
/**
 * @file AllocatorRegistry.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 31, 2026
 *
 * @brief Simple data structure for tracking allocators.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <cstdint>


/**
 * @addtogroup PMM_Core
 * @{
 */

namespace pmm
{
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
} // namespace pmm

/** @} */
